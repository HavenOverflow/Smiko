REPO_ROOT=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# LibSTDC++
LIBCPP  = -lstdc++

# LibUSB
LIBUSB_CFLAGS  = $(shell $(PKG_CONFIG) libusb-1.0 --cflags)
ifeq ($(STATIC),)
LIBUSB += $(shell $(PKG_CONFIG) libusb-1.0 --libs)
else
LIBUSB += $(REPO_ROOT)/lib/$(ARCH)/libusb-1.0.a
endif

# LibFTDI
# This can either be libftdi1 or libftdi, depending on the system. We prefer libftdi1, but if it's not available, we'll fall back to libftdi.
LIBFTDI_CFLAGS  = $(shell $(PKG_CONFIG) --cflags libftdi1 2>/dev/null || $(PKG_CONFIG) --cflags libftdi)
ifeq ($(STATIC),)
LIBFTDI += $(shell $(PKG_CONFIG) libftdi1 --libs 2>/dev/null || $(PKG_CONFIG) libftdi --libs)
else
LIBFTDI += $(REPO_ROOT)/lib/$(ARCH)/libftdi1.a
endif

# LibCrypto
LIBCRYPTO_CFLAGS  = $(shell $(PKG_CONFIG) libcrypto --cflags)
ifeq ($(STATIC),)
LIBCRYPTO += $(shell $(PKG_CONFIG) libcrypto --libs)
else
LIBCRYPTO += $(REPO_ROOT)/lib/$(ARCH)/libcrypto.a
endif

# LibProtobuf
LIBPROTOBUF_CFLAGS  = $(shell $(PKG_CONFIG) protobuf-lite --cflags)
ifeq ($(STATIC),)
LIBPROTOBUF += $(shell $(PKG_CONFIG) protobuf-lite --libs)
else
LIBPROTOBUF += $(REPO_ROOT)/lib/$(ARCH)/libprotobuf-lite.a
endif

# LibTss2
LIBTSS2_CFLAGS  = $(shell $(PKG_CONFIG) tss2-sys --cflags)
LIBTSS2_CFLAGS += $(shell $(PKG_CONFIG) tss2-mu --cflags)
LIBTSS2_CFLAGS += $(shell $(PKG_CONFIG) tss2-rc --cflags)
ifeq ($(STATIC),)
LIBTSS2 += $(shell $(PKG_CONFIG) tss2-sys --libs)
LIBTSS2 += $(shell $(PKG_CONFIG) tss2-mu --libs)
LIBTSS2 += $(shell $(PKG_CONFIG) tss2-rc --libs)
else
LIBTSS2 += $(REPO_ROOT)/lib/$(ARCH)/libtss2-sys.a
LIBTSS2 += $(REPO_ROOT)/lib/$(ARCH)/libtss2-mu.a
LIBTSS2 += $(REPO_ROOT)/lib/$(ARCH)/libtss2-rc.a
endif

# LibELF
LIBELF_CFLAGS  = $(shell $(PKG_CONFIG) libelf --cflags)
ifeq ($(STATIC),)
LIBELF += $(shell $(PKG_CONFIG) libelf --libs)
else
LIBELF += $(REPO_ROOT)/lib/$(ARCH)/libelf.a
endif

# LibLZMA
LIBLZMA_CFLAGS  = $(shell $(PKG_CONFIG) liblzma --cflags)
ifeq ($(STATIC),)
LIBLZMA += $(shell $(PKG_CONFIG) liblzma --libs)
else
LIBLZMA += $(REPO_ROOT)/lib/$(ARCH)/liblzma.a
endif

# LibXML2
LIBXML2_CFLAGS  = $(shell $(PKG_CONFIG) libxml-2.0 --cflags)
ifeq ($(STATIC),)
LIBXML2 += $(shell $(PKG_CONFIG) libxml-2.0 --libs)
else
LIBXML2 += $(REPO_ROOT)/lib/$(ARCH)/libxml-2.0.a
endif

# LibZ
LIBZ_CFLAGS  = $(shell $(PKG_CONFIG) zlib --cflags)
ifeq ($(STATIC),)
LIBZ += $(shell $(PKG_CONFIG) zlib --libs)
else
LIBZ += $(REPO_ROOT)/lib/$(ARCH)/libz.a
endif

# This requires a VERY specific order.
LIBS = $(LIBCPP) $(LIBUSB) $(LIBFTDI) $(LIBCRYPTO) $(LIBPROTOBUF) $(LIBTSS2) $(LIBELF) $(LIBXML2) $(LIBZ) $(LIBLZMA)

LIBUSB += $(LIBUSB_CFLAGS)
LIBFTDI += $(LIBFTDI_CFLAGS)
LIBCRYPTO += $(LIBCRYPTO_CFLAGS)
LIBPROTOBUF += $(LIBPROTOBUF_CFLAGS)
LIBTSS2 += $(LIBTSS2_CFLAGS)
LIBELF += $(LIBELF_CFLAGS)
LIBXML2 += $(LIBXML2_CFLAGS)
LIBZ += $(LIBZ_CFLAGS)
LIBLZMA += $(LIBLZMA_CFLAGS)

LIBS_CFLAGS = $(LIBUSB_CFLAGS) $(LIBFTDI_CFLAGS) $(LIBCRYPTO_CFLAGS) $(LIBPROTOBUF_CFLAGS) $(LIBTSS2_CFLAGS) $(LIBELF_CFLAGS) $(LIBXML2_CFLAGS) $(LIBZ_CFLAGS) $(LIBLZMA_CFLAGS)

# Static compilation handler
ifneq ($(STATIC),)
CFLAGS += -static
CXXFLAGS += -static
LDFLAGS += -static
endif

ifdef HEADER_DIR
CFLAGS += -I$(HEADER_DIR)
CXXFLAGS += -I$(HEADER_DIR)
LDFLAGS += -L$(HEADER_DIR)
endif