/* Copyright 2025 HavenOverflow
 * Use of this code is permissible so long as the appropriate credit
 * is provided. See the LICENSE file for more info.
 */

#ifndef __SMIKO_COMMON_H
#define __SMIKO_COMMON_H

#ifndef ARRAY_LEN
#define ARRAY_LEN(x) \
    sizeof(x) / sizeof(x[0])
#endif

#define SHA256_DIGEST_WORDS (SHA256_DIGEST_LENGTH / sizeof(uint32_t))

#define MIN(a, b) \
({ \
    __typeof__(a) temp_a = (a);	\
    __typeof__(b) temp_b = (b); \
    \
    temp_a < temp_b ? temp_a : temp_b; \
})

#endif /* __SMIKO_COMMON_H */