/* Copyright 2025 HavenOverflow
 * Use of this code is permissible so long as the appropriate credit
 * is provided. See the LICENSE file for more info.
 */

#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "args.h"
#include "chip_config.h"
#include "ihex.h"
#include "signed_header.h"


bool valid_signed_header(const struct SignedHeader *header)
{
	if (header->magic != MAGIC_HAVEN && 
		header->magic != MAGIC_CITADEL &&
		header->magic != MAGIC_DAUNTLESS)
		return false;

	if (header->image_size < 0x800)
		return false;

	/*
	 * Both Rx base and Ro base are the memory mapped address, but they
	 * should have the same offset. The rx section starts after the header.
	 */
	if (header->rx_base != header->ro_base + sizeof(struct SignedHeader))
		return false;

	return true;
}

bool valid_signed_manifest(const struct SignedManifest *manifest)
{
	if (manifest->identifier != ID_ROM_EXT && manifest->identifier != ID_OWNER_FW)
		return false;

	if (manifest->code_start > manifest->code_end)
		return false;

	if (manifest->code_start > manifest->image_size)
		return false;

	if (manifest->code_end > manifest->image_size)
		return false;

	if (manifest->entry_point > manifest->code_end)
		return false;

	if (manifest->entry_point < manifest->code_start)
		return false;

	return true;
}

/* Rounds and address up to the next 2KB boundary if not one already */
static inline uint32_t round_up_2kb(const uint32_t addr)
{
	const uint32_t mask = (2 * 1024) - 1;

	return (addr + mask) & ~mask;
}

/* Returns the RW header or -1 if one cannot be found */
uint32_t find_rw_header(const void *data, size_t size, uint32_t offset)
{
	offset = round_up_2kb(offset);

	while (offset < size) {
		if (valid_signed_header((const struct SignedHeader *)(data + offset)))
			return offset;
		if (valid_signed_manifest((const struct SignedManifest *)(data + offset)))
			return offset;
		offset = round_up_2kb(offset + 1);
	}

	return -1;
}

static uint8_t *read_file(const char *path, size_t *size)
{
    FILE *fp;
	struct stat st;
	
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
	*size = st.st_size;
	
    return data;
}

void show_info(int esc)
{
    printf("Usage: %s -i [/path/to/gsc-firmware.bin] <optional args>\n"
        "\n"
        "Arguments:\n"
        "-h, --help: Show this help and exit.\n"
        "-s, --section [ro/rw]: Specify the section to carve out of the image. (Default: 'rw')\n"
        "-k, --keep_header: Keep the signed header in the extracted payload.\n"
        "-i, --image [/path/to/*50.bin.prod]: Specify the path of the input firmware file.\n"
        "-o, --out [/path/to/output.flat]: Specify the path to the output binary payload. (Default: './output.flat')\n"
        "-d, --defaults: Always assume the target image is unsigned and use the default size values.\n"
        "-b, --section_b: Carve out from the B sections of the file.\n"
        "-u, --unsign: Remove the Signed Header/Manifest from the extracted binary.\n"
        "-v, --verbose: Show extra debug output.\n"
        "\n", gargv[0]);
    exit(esc);
}

int main(int argc, char **argv)
{
    gargc = argc;
    gargv = argv;
    if (argc < 2) {
        fprintf(stderr, "Error: Expected more arguments.\n");
        show_info(1);
    }
    if (fbool("--help","-h")) show_info(0);
    if (!fbool("--image","-i")) {
        fprintf(stderr, "Error: Expected --image as an argument.\n");
        show_info(1);
    }

    const char *path = fval("--image", "-i", 1);
    const char *out = (fbool("--out","-o")) ? fval("--out", "-o", 1) : "./output.flat";
    size_t data_size;
    void *data = read_file(path, &data_size);
    void *data_ptr = data;
    struct SignedHeader *hdr;
    struct SignedManifest *man;

    printf("Info: Read firmware successfully, carving out requested section.\n");

    if (fbool("--unsign","-u")) {
        data += sizeof(struct SignedHeader);
        data_size -= sizeof(struct SignedHeader);

        FILE *fp = fopen(path, "w+");
        fwrite(data, data_size, 1, fp);
        fclose(fp);

        printf("Info: Target payload unsigned successfully.\n");

        return 0;
    }

    size_t offset, size;

    if (fbool("--section_b","-b")) data_ptr += (data_size > CONFIG_FLASH_SIZE) ? CFG_DT_FLASH_HALF : CFG_FLASH_HALF; // Skip to the second half of the file to the B sections.

    if (!fbool("--section","-s")) {
        // Default to the RW header
        select_rw:
        if (!fbool("--defaults","d"))
            offset = find_rw_header(data_ptr, data_size, CONFIG_RO_SIZE);
        else
            offset = (data_size > CONFIG_FLASH_SIZE) ? CONFIG_RO_SIZE + 2048 : CONFIG_RO_SIZE;
    }else{
        if (!strcmp(fval("--section", "-s", 1), "rw")) {
            goto select_rw;
        }else{
            offset = 0; // The RO firmware is always at 0x0
        }
    }

    if (offset < 0) {
        fprintf(stderr, "Error: Failed to find a valid RW header.\n");
        free(data);
        return 1;
    }

    hdr = (struct SignedHeader *)(data_ptr + offset);
    man = (struct SignedManifest *)(data_ptr + offset);
    if (!valid_signed_header(hdr) && !valid_signed_manifest(man) && !fbool("--defaults","-d")) {
        fprintf(stderr, "Error: Image does not contain a valid header or manifest.\n");
        return 1;
    }

    size = (valid_signed_header(hdr)) ? (hdr->image_size) : (man->code_end - man->code_start);

    /* This should only happen if the header is unsigned, in which case 
     * it will all be 0xFFFFFFFF, and the subtracted size will always be 0. 
     */
    if (!size || fbool("--defaults","-d")) {
        printf("Info: Image header/manifest is not populated, using default size values.\n");
        
        if (fbool("--section", "-s")) {
            if (!strcmp(fval("--section", "-s", 1), "ro")) {
                size = (data_size > CONFIG_FLASH_SIZE) ? CONFIG_RO_SIZE : CONFIG_RO_SIZE;
            }else{
                size = (data_size > CONFIG_FLASH_SIZE) ? CONFIG_DT_RW_SIZE : CONFIG_RW_SIZE;
            }
        }else{
            size = (data_size > CONFIG_FLASH_SIZE) ? CONFIG_DT_RW_SIZE : CONFIG_RW_SIZE;
        }
    }

    
    if (!fbool("--keep_header","-k")) {
        offset += sizeof(struct SignedHeader);
        size -= sizeof(struct SignedHeader);
    }

    if (fbool("--verbose","-v")) printf("Debug: Carving from offset 0x%lx to 0x%lx\n", offset, offset + size);

    uint8_t *payload = malloc(size);
    memcpy(payload, data_ptr + offset, size);
    free(data);

    // We need tailing 0xff bytes to maintain the image signature.
    if (!fbool("--keep_header","-k")) 
        while (size && (payload[size - 1] == 0xff)) size--; // Cut off tailing 0xFF bytes

    printf("Info: Section carved out successfully, writing to file.\n");
    FILE *fd = fopen(out, "w+");
    fwrite(payload, size, 1, fd);
    fclose(fd);

    return 0;
}
