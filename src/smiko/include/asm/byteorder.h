// macOS compatibility layer because gnu99 standard differs from c99 standard
#pragma once

#ifdef __APPLE__
#include <stdint.h>
#include "endian.h"

typedef uint8_t  __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef uint16_t __be16;
typedef uint32_t __be32;
typedef uint64_t __be64;

typedef uint16_t __le16;
typedef uint32_t __le32;
typedef uint64_t __le64;
#else
#include_next <asm/byteorder.h>
#endif
