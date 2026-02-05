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
#include <tss2/tss2_rc.h>
#include <tss2/tss2_sys.h>
#include <tss2/tss2_tpm2_types.h>
#include <unistd.h>

#include "args.h"
#include "crypto.h"
#include "nvmem.hh"
#include "rmasmoke.hh"
#include "tss_context.h"

extern int tpm; // Global file descriptor to /dev/tpm0, defined in rmasmoke.cc

// TPM packet constructors.
TSS2_SYS_CONTEXT *GenerateContext(size_t *size) 
{
	const size_t contextSize = Tss2_Sys_GetContextSize(0);
	TSS2_SYS_CONTEXT *sysctx = (TSS2_SYS_CONTEXT *)calloc(1, contextSize);
	
	memset(sysctx, 0, contextSize);
	((TSS2_SYS_CONTEXT_INT *)sysctx)->cmdBuffer = (UINT8 *)((uintptr_t)sysctx + sizeof(TSS2_SYS_CONTEXT_INT));
	((TSS2_SYS_CONTEXT_INT *)sysctx)->maxCmdSize = contextSize - sizeof(TSS2_SYS_CONTEXT_INT);
	
	if (size) *size = contextSize;
	
	return sysctx;
}

struct tpm_result *GetCommandBufferFromSys(TSS2_SYS_CONTEXT *ctx) 
{
	return (tpm_result *)(((TSS2_SYS_CONTEXT_INT *)ctx)->cmdBuffer);
}

void ClearTPMContext(TSS2_SYS_CONTEXT *ctx) 
{
	size_t len = Tss2_Sys_GetContextSize(0) - sizeof(TSS2_SYS_CONTEXT_INT);
	memset((void *)(&((TSS2_SYS_CONTEXT_INT *)ctx)[1]), 0, len);
	(*(((uint8_t *)ctx) + 0x40)) = CMD_STAGE_INITIALIZE;
}

void PreComplete(TSS2_SYS_CONTEXT *sysctx) 
{
	(*(((uint8_t *)sysctx) + 0x40)) = CMD_STAGE_RECEIVE_RESPONSE;
}



// Function to start the TPM.
int tpm_startup(void)
{
	size_t size, resp_len = 0;
	int rc = TPM2_RC_SUCCESS;
	TSS2_SYS_CONTEXT *sysctx = GenerateContext(&size);
	struct tpm_result *packet = GetCommandBufferFromSys(sysctx);
	struct tpm_result *resultPtr = nullptr;

	Tss2_Sys_Startup_Prepare(sysctx, TPM2_SU_CLEAR);
	PreComplete(sysctx);
	rc = SendTPMCommand(packet, htobe32(packet->length), &resultPtr, &resp_len, tpm);
	if (rc != TPM2_RC_SUCCESS) {
		std::cerr << "Error: Failed to send startup command to TPM2." << std::endl;
		std::cerr << "TPM Response: " << Tss2_RC_Decode(rc) << std::endl;
		return -1;
	}

	return 0;
}

// Function to define an NV space.
int nvmem_define(uint32_t index, TPM2B_NV_PUBLIC *pub)
{
	size_t size, resp_len = 0;
	int rc = TPM2_RC_SUCCESS;
	TSS2_SYS_CONTEXT *sysctx = GenerateContext(&size);
	TSS2L_SYS_AUTH_COMMAND cmd = generate_auth_token();
	struct tpm_result *packet = GetCommandBufferFromSys(sysctx);
	struct tpm_result *resultPtr = nullptr;
	const char *passwd = "default password";
	
	TPM2B_AUTH auth;
	auth.size = SHA256_DIGEST_LENGTH;
    SHA256((uint8_t *)passwd, strlen(passwd), auth.buffer);
	
    // Handle the define with libtss2-sys
	Tss2_Sys_NV_DefineSpace_Prepare(sysctx, TPM2_RH_OWNER, &auth, pub);
	Tss2_Sys_SetCmdAuths(sysctx, &cmd); // Authorize the command using the TPM owner password
	PreComplete(sysctx);
	rc = SendTPMCommand(packet, htobe32(packet->length), &resultPtr, &resp_len, tpm);
	if (rc != TPM2_RC_SUCCESS) {
		std::cerr << "Error: Failed to define TPM space in TPM2." << std::endl;
		std::cerr << "TPM Response: " << Tss2_RC_Decode(rc) << std::endl;
		return -1;
	}

	return 0;
}

// Function to undefine an NV space.
int nvmem_undefine(uint32_t index)
{
	size_t size, resp_len = 0;
	int rc = TPM2_RC_SUCCESS;
	TSS2_SYS_CONTEXT *sysctx = GenerateContext(&size);
	TSS2L_SYS_AUTH_COMMAND cmd = generate_auth_token();
	struct tpm_result *packet = GetCommandBufferFromSys(sysctx);
	struct tpm_result *resultPtr = nullptr;
	const char *passwd = "default password";
	
	TPM2B_AUTH auth;
	auth.size = SHA256_DIGEST_LENGTH;
    SHA256((uint8_t *)passwd, strlen(passwd), auth.buffer);

    // Handle the undefine with libtss2-sys
	Tss2_Sys_NV_UndefineSpace_Prepare(sysctx, TPM2_RH_OWNER, TPM2_NV_INDEX_FIRST + index);
	Tss2_Sys_SetCmdAuths(sysctx, &cmd); // Authorize the command using the TPM owner password
	PreComplete(sysctx);
	rc = SendTPMCommand(packet, htobe32(packet->length), &resultPtr, &resp_len, tpm);
	if (rc != TPM2_RC_SUCCESS) {
		std::cerr << "Error: Failed to undefine TPM space in TPM2." << std::endl;
		std::cerr << "TPM Response: " << Tss2_RC_Decode(rc) << std::endl;
		return -1;
	}

	return 0;
}

// Function to write to an NV space.
void *nvmem_write(uint32_t index, uint32_t offset,
                  void *data, size_t len)
{
	size_t size, resp_len = 0;
	TSS2_SYS_CONTEXT *sysctx = GenerateContext(&size);
	TSS2L_SYS_AUTH_COMMAND cmd = generate_auth_token();
	struct tpm_result *packet = GetCommandBufferFromSys(sysctx);
	struct tpm_result *resultPtr = nullptr;

	if (fbool("verbose") && !(len & 3)) {
		std::cout << "Debug: " << __func__ << ": Writing ";

		uint32_t *ptr = (uint32_t *)(data);
		for (int i = 0; i < (len / sizeof(uint32_t)); ++i)
			printf("0x%x ", ptr[i]);
		std::cout << std::endl;
	}

    // Copy our data into the NV buffer
	TPM2B_MAX_NV_BUFFER nvbuf;
	memcpy(nvbuf.buffer, data, len);
	nvbuf.size = len;
	
    // Handle the write with libtss2-sys
	Tss2_Sys_NV_Write_Prepare(sysctx, TPM2_RH_OWNER, TPM2_NV_INDEX_FIRST + index, &nvbuf, offset);
	Tss2_Sys_SetCmdAuths(sysctx, &cmd); // Authorize the command using the TPM owner password
	PreComplete(sysctx);
	if (SendTPMCommand(packet, htobe32(packet->length), &resultPtr, &resp_len, tpm) != 0) {
		std::cerr << "Error: Failed to write to NVMemory in TPM2." << std::endl;
		return nullptr;
	}

	return data;
}

// Function to read from an NV space.
void *nvmem_read(uint32_t index, uint16_t offset, 
                 uint16_t length)
{	
	size_t size, resp_len = 0;
	TSS2_SYS_CONTEXT *sysctx = GenerateContext(&size);
	TSS2L_SYS_AUTH_COMMAND cmd = generate_auth_token();
	struct tpm_result *packet = GetCommandBufferFromSys(sysctx);
	struct tpm_result *resultPtr = nullptr;
	TPM2B_MAX_NV_BUFFER *nvbuf;
	
    // Handle the read with libtss2-sys
	Tss2_Sys_NV_Read_Prepare(sysctx, TPM2_RH_OWNER, TPM2_NV_INDEX_FIRST + index, length, offset);
	Tss2_Sys_SetCmdAuths(sysctx, &cmd); // Authorize the command using the TPM owner password
	PreComplete(sysctx);
	if (SendTPMCommand(packet, htobe32(packet->length), &resultPtr, &resp_len, tpm) != 0) {
		std::cerr << "Error: Failed to read from NVMemory in TPM2." << std::endl;
		return NULL;
	}
	Tss2_Sys_NV_Read_Complete(sysctx, nvbuf);

	return &nvbuf->buffer;
}

// Function to clear all NV memory in the TPM.
int nvmem_clear(void)
{
	/*size_t size, resp_len = 0;
	TSS2_SYS_CONTEXT *sysctx = GenerateContext(&size);
	struct tpm_result *packet = GetCommandBufferFromSys(sysctx);
	struct tpm_result *resultPtr = nullptr;

    // Handle the clear with libtss2-sys
	Tss2_Sys_Clear_Prepare(sysctx, TPMI_RH_CLEAR);
	PreComplete(sysctx);
	return SendTPMCommand(packet, htobe32(packet->length), &resultPtr, &resp_len, tpm);*/

	return 0;
}

int nvmem_set_hierarchy(std::string owner_password,
                        std::string endorsement_password,
					    std::string lockout_password)
{
	size_t size, resp_len = 0;
	TSS2_SYS_CONTEXT *sysctx = GenerateContext(&size);
	struct tpm_result *packet = GetCommandBufferFromSys(sysctx);
	struct tpm_result *resultPtr = nullptr;
	TPM2B_AUTH auth;

	TSS2L_SYS_AUTH_COMMAND cmdAuths = start_auth_session();

	auth.size = (UINT16)strlen(owner_password.c_str());
	memcpy(auth.buffer, owner_password.c_str(), auth.size);
	Tss2_Sys_HierarchyChangeAuth_Prepare(sysctx, TPM2_RH_OWNER, &auth);
	Tss2_Sys_SetCmdAuths(sysctx, &cmdAuths);
	PreComplete(sysctx);
	if (SendTPMCommand(packet, htobe32(packet->length), &resultPtr, &resp_len, tpm) != 0) {
		std::cerr << "Error: Failed to set Owner Password in TPM2." << std::endl;
		return -1;
	}

	auth.size = (UINT16)strlen(endorsement_password.c_str());
	memcpy(auth.buffer, endorsement_password.c_str(), auth.size);
	Tss2_Sys_HierarchyChangeAuth_Prepare(sysctx, TPM2_RH_ENDORSEMENT, &auth);
	Tss2_Sys_SetCmdAuths(sysctx, &cmdAuths);
	PreComplete(sysctx);
	if (SendTPMCommand(packet, htobe32(packet->length), &resultPtr, &resp_len, tpm) != 0) {
		std::cerr << "Error: Failed to set Endorsement Password in TPM2." << std::endl;
		return -1;
	}

	auth.size = (UINT16)strlen(lockout_password.c_str());
	memcpy(auth.buffer, lockout_password.c_str(), auth.size);
	Tss2_Sys_HierarchyChangeAuth_Prepare(sysctx, TPM2_RH_LOCKOUT, &auth);
	Tss2_Sys_SetCmdAuths(sysctx, &cmdAuths);
	PreComplete(sysctx);
	if (SendTPMCommand(packet, htobe32(packet->length), &resultPtr, &resp_len, tpm) != 0) {
		std::cerr << "Error: Failed to set Lockout Password in TPM2." << std::endl;
		return -1;
	}

	return 0;
}