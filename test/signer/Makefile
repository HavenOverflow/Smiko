ARCH ?= $(shell uname -m)
CROSS_COMPILE ?=
SHELL ?= /bin/sh
PKG_CONFIG ?= pkg-config
CXX ?= $(CROSS_COMPILE)g++
CFLAGS := -I../../src -I../../include -I../../build/gen/include -Icommon -Wno-deprecated-declarations \
	-DHAVE_JSON=1

INCLUDES += $(shell $(PKG_CONFIG) libxml-2.0 --cflags)
INCLUDES += $(shell $(PKG_CONFIG) libcrypto --cflags)
INCLUDES += $(shell $(PKG_CONFIG) libssl --cflags)
INCLUDES += $(shell $(PKG_CONFIG) libelf --cflags)

ifeq ($(STATIC),)
LIBS += $(shell $(PKG_CONFIG) libxml-2.0 --libs)
LIBS += $(shell $(PKG_CONFIG) libcrypto --libs)
LIBS += $(shell $(PKG_CONFIG) libssl --libs)
LIBS += $(shell $(PKG_CONFIG) libelf --libs)
LIBS += $(shell $(PKG_CONFIG) libusb-1.0 --libs)
else
LIBS += ../../lib/$(ARCH)/libxml-2.0.a
LIBS += ../../lib/$(ARCH)/libcrypto.a
LIBS += ../../lib/$(ARCH)/libssl.a
LIBS += ../../lib/$(ARCH)/libelf.a
LIBS += ../../lib/$(ARCH)/libusb-1.0.a
LIBS += ../../lib/$(ARCH)/libz.a
LIBS += ../../lib/$(ARCH)/liblzma.a
CFLAGS += -static
endif

ODIR ?= build/$(ARCH)

ifeq ($(VERBOSE),)
Q := @
else
Q := 
endif

SCRIPTS := codesigner.cc aes.cc ecdh.cc image.cc publickey.cc gnubby.cc


all: $(ODIR) $(ODIR)/cr50-codesigner

$(ODIR):
	$(Q)mkdir -p $@

$(ODIR)/cr50-codesigner: $(SCRIPTS)
	$(Q)mkdir -p ../../build/gen/include
	@echo "  PMJP    pmjp.h"
	$(Q)./pmjp.py ec_RW-manifest-prod.json >../../build/gen/include/pmjp.h
	@echo "  CXX     build/bin/$(ARCH)/cr50-codesigner"
	$(Q)$(CXX) $^ -o $@ $(CFLAGS) $(INCLUDES) $(LIBS)

clean:
	rm -rf build
