// macOS compatibility layer because gnu99 standard differs from c99 standard
#pragma once

#ifdef __APPLE__
#include <stdint.h>
#include <libkern/OSByteOrder.h>

#ifndef htobe16
#define htobe16(x) OSSwapHostToBigInt16(x)
#endif
#ifndef htobe32
#define htobe32(x) OSSwapHostToBigInt32(x)
#endif
#ifndef htobe64
#define htobe64(x) OSSwapHostToBigInt64(x)
#endif

#ifndef be16toh
#define be16toh(x) OSSwapBigToHostInt16(x)
#endif
#ifndef be32toh
#define be32toh(x) OSSwapBigToHostInt32(x)
#endif
#ifndef be64toh
#define be64toh(x) OSSwapBigToHostInt64(x)
#endif

#ifndef htole16
#define htole16(x) OSSwapHostToLittleInt16(x)
#endif
#ifndef htole32
#define htole32(x) OSSwapHostToLittleInt32(x)
#endif
#ifndef htole64
#define htole64(x) OSSwapHostToLittleInt64(x)
#endif

#ifndef le16toh
#define le16toh(x) OSSwapLittleToHostInt16(x)
#endif
#ifndef le32toh
#define le32toh(x) OSSwapLittleToHostInt32(x)
#endif
#ifndef le64toh
#define le64toh(x) OSSwapLittleToHostInt64(x)
#endif

#else
#include_next <endian.h>
#endif
