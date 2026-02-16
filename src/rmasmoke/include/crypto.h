/* Hannah's shitty GSCTool crypto implementation!
 * Most of it's intended for Cr50/Ti50 firmware upgrades
 * and image signing, but other than that it provides some
 * simple functions for convenience.
*/

#ifndef __SMIKO_CRYPTO_H
#define __SMIKO_CRYPTO_H
#define SHA256_DIGEST_WORDS (SHA256_DIGEST_LENGTH / sizeof(uint32_t))
#define MIN(a, b) \
({ \
    __typeof__(a) temp_a = (a);	\
    __typeof__(b) temp_b = (b); \
    \
    temp_a < temp_b ? temp_a : temp_b; \
})
#endif /* __SMIKO_CRYPTO_H */