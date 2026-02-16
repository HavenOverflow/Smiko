# repo_root is passed from main Makefile
include $(REPO_ROOT)/toolchain.mk
include $(REPO_ROOT)/sources.mk

CFLAGS += -I../../src -I../../include -I../../build/gen/include -Icommon -Wno-deprecated-declarations \
	-DHAVE_JSON=1

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
	$(Q)$(CXX) $^ -o $@ $(CFLAGS) $(INCLUDES) $(LIBS) $(LDFLAGS)

clean:
	rm -rf build
