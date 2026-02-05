/* Copyright 2025 HavenOverflow
 * Use of this code is permissible so long as the appropriate credit
 * is provided. See the LICENSE file for more info.
 */

#ifndef __SMIKO_INCLUDE_ARGS_H
#define __SMIKO_INCLUDE_ARGS_H

#ifdef __cplusplus
extern "C" {
#endif

int fbool(char *arg);

char *fval(char *arg, int sel);

void parse_args(int argc, char **argv);

void show_info(int esc);

void show_ver(int esc);

#ifdef __cplusplus
}
#endif

#endif /* __SMIKO_INCLUDE_ARGS_H */