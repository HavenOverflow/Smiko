/* Copyright 2025 HavenOverflow
 * Use of this code is permissible so long as the appropriate credit
 * is provided. See the LICENSE file for more info.
 */

#include <asm/byteorder.h>
#include <ctype.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "args.h"
#include "board_id.h"
#include "chip_config.h"
#include "compile_time_macros.h"
#include "common.h"
#include "console.h"
#include "sysinfo.h"
#include "smiko.h"
#include "signed_header.h"
#include "trunks.h"
#include "tpm.h"
#include "tpm_vendor_cmds.h"
#include "usb.h"
#include "vendor_cmds.h"
#include "verify.h"
#include "version.h"


enum transfer_type xfer_type;
extern union gsc_start_resp start_resp;

int open_socket(void)
{
	int i, dev;
	// Search for all of these sockets in descending priority.
	const char *devices[] = {
		"/dev/tpm0",      // Cr50 & Ti50
		"/dev/gsc0",      // Acropora
		"/dev/citadel0",  // Nugget-OS
		"/dev/swtpm0",    // tpm2-simulator
	};
	BUILD_ASSERT(ARRAY_LEN(devices) == XFER_METHOD_COUNT);

	for (i = 0; i < ARRAY_LEN(devices); ++i) {
		dev = open(devices[i], O_RDWR);
		
		/* We found our socket, we're done. */
		if (dev > -1) {
			xfer_type = i; // Keep alinged with transfer_type enum in smiko.h! ~ Hannah
			return dev;
		}
	}

	return -1;
}

uint8_t *read_file(const char *path, uint32_t *size_ptr)
{
	FILE *fp;
	struct stat st;

	if (fbool("verbose")) printf("Debug: Copying contents of file %s to memory.\n", path); 
	
	fp = fopen(path, "rb");
	if (!fp) {
		fprintf(stderr, "Error: Failed to open file '%s'. %s\n", path, strerror(errno));
		exit(1);
	}
	
	if (fstat(fileno(fp), &st)) {
		fprintf(stderr, "Error: Failed to stat file '%s'. %s\n", path, strerror(errno));
		exit(1);
	}

	void *data = malloc(st.st_size);
	if (!data) {
		fprintf(stderr, "Error: Failed to allocate memory for provided file '%s'. %s\n", path, strerror(errno));
		exit(1);
	}

	if (fread(data, st.st_size, 1, fp) != 1) {
		fprintf(stderr, "Error: Failed to read file '%s'. %s\n", path, strerror(errno));
		exit(1);
	}
	
	fclose(fp);

	*size_ptr = st.st_size;
	
	return data;
}

int main(int argc, char **argv)
{
	parse_args(argc, argv);
	
	if (argc < 2) {
		fprintf(stderr, "Error: Expected more arguments.\n");
		show_info(1);
	} 
	if (fbool("help")) show_info(0);
	if (fbool("version")) show_ver(0);

	if (fbool("headerinfo")) {
		get_headerinfo(fval("headerinfo"));
		return 0;
	}

	if (fbool("dump")) {
		get_keyinfo(fval("dump"));
		return 0;
	}

	if (fbool("verify")) {
		const char *file = fval("verify");
		uint32_t size;

		void *data = read_file(file, &size);
		verify_image(data, size);
		return 0;
	}

	/* Everything below this point needs root access to operate. */
	if (getuid() != 0) {
		fprintf(stderr, "Error: Please run %s as root.\n", argv[0]);
		exit(1);
	}

	if (fbool("console")) {
		launch_console(fval("console"));
		return 0;
	}

	if (connect_to_target() != 0) return 1;
	
	if (fbool("sysinfo")) {
		if (get_sysinfo(start_resp.rpdu) != 0) {
			fprintf(stderr, "Error: Failed to fetch system information from TPM.\n");
			return 1;
		}
	}

	if (fbool("flash")) {
		if (write_firmware(fval("flash")) != 0) {
			fprintf(stderr, "Error: Failed to write provided firmware image.\n");
			return 1;
		}else{
			printf("Info: Successfully wrote provided firmware image.\n");
		}
	}

	if (fbool("reset")) reset_gsc();

	if (!fbool("suzyq") && !fbool("trunks")) {
		if (fbool("verbose")) 
			printf("Debug: Closing connection to target.\n");
		close(gsc_socket);
	}

	return 0;
}
