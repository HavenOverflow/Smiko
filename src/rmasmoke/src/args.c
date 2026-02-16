/* Copyright 2025 HavenOverflow
 * Use of this code is permissible so long as the appropriate credit
 * is provided. See the LICENSE file for more info.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "args.h"
#include "common.h"
#include "version.h"

static int gargc;
static char **gargv;


/* TODO (Hannah): Finish the new argparse impl. */
enum arg_type {
	ARG_BOOLEAN,
	ARG_STRING
};

struct cmd_line_args {
	char shorthand;
	char *arg;
	char *params;
	char *desc;
	enum arg_type type;
};

const struct cmd_line_args args[] = {
	{'h', "help", NULL, "Show this help and exit", ARG_BOOLEAN},
	{'v', "verbose", NULL, "Display debug logs on stdout", ARG_BOOLEAN},
	{'V', "version", " [0.0.0]", "Specify the Cr50 version to test under", ARG_STRING},
	{'s', "socket", " /dev/tpm0", "Specify a chardev instead of /dev/tpm0", ARG_STRING},
	{'L', "local_data_path", " [path]", "Specify the path to the local_tpm_data to be used in authorization", ARG_STRING},
	{'u', "startup", NULL, "Start TPM2 if it hasn't already been started (e.g. for emulators)", ARG_BOOLEAN},
	{'S', "setup", NULL, "Setup the necessary index for RMASmoke to operate", ARG_BOOLEAN},
	{'c', "cleanup", NULL, "Clean up indexes created by RMASmoke", ARG_BOOLEAN},
	{'o', "take_ownership", NULL, "Take ownership of TPM2 and generate local_tpm_data", ARG_BOOLEAN},
	{'g', "dglobalsec", NULL, "Disarm GLOBALSEC and jump to RW_B", ARG_BOOLEAN},
	{'a', "dump_addr", " <addr> <length>", "Specify an address to dump to the CCD console", ARG_STRING},
};

int fbool(char *arg)
{
	char buf[strlen(arg) + 3];
	int i, argoff = -1;

	/* Find our argument definition. */
	for (i = 0; i < ARRAY_LEN(args); ++i) {
		if (!strcmp(arg, args[i].arg)) {
			argoff = i;
			break;
		}
	}

	strcpy(&buf[2], arg);
	buf[0] = buf[1] = '-';

	for (i = 0; i < gargc; ++i) {
		if (!strcmp(buf, gargv[i]))
			return i;
	}

	if (argoff != -1) {
		buf[1] = args[argoff].shorthand;
		buf[2] = 0; // NULL
		for (i = 0; i < gargc; ++i) {
			if (!strcmp(buf, gargv[i])) 
				return i;
		}
	}

	return 0;
}

char *fval(char *arg, int sel)
{
	int offset = fbool(arg);
	if (!offset || (gargc - offset) < 2)
		return NULL;

	if (gargv[offset + 1][0] == '-') {
		fprintf(stderr, "Error: --%s expects an argument!\n", arg);
		exit(1);
	}
	
	return gargv[offset + sel];
}

void parse_args(int argc, char **argv)
{
	gargc = argc;
	gargv = argv;

	// TODO (Hannah): Check for invalid arguments here
}

void show_info(int esc)
{
	int i;

	printf("RMASmoke - TCG TPM 2.0 reference buffer overflow\n"
		"\n"
		"Usage: sudo %s [args]"
		"\n"
		"RMASmoke is a tool (written by Writable and greatly improved by Hannah) designed to exploit\n"
		"a stack buffer overflow present in TPM2, which is utilized by the Cr50 firmware present on\n"
		"nearly every Chromebook manufactured since 2019, with the goal of using return-oriented-programming\n"
		"to gain out-of-bounds read/write in the H1 chip, and use this control to read on-board secrets\n"
		"(e.g. the BootROM) or modify important values (e.g. Firmware headers, RMA auth, and hardware registers)\n"
		"\n"
		"Arguments:\n",
		gargv[0]);
	for (i = 0; i < ARRAY_LEN(args); ++i) {
		printf("-%c, --%s%s: %s\n", args[i].shorthand, args[i].arg,
				(args[i].params) ? args[i].params : "", args[i].desc);
	}

	exit(esc);
}