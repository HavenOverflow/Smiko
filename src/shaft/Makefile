# repo_root is passed from main Makefile
include $(REPO_ROOT)/toolchain.mk
include $(REPO_ROOT)/sources.mk

BDIR ?= build/$(ARCH)

# These are the flags used to compile normal binaries throughout src/*.c
CPPFLAGS += -Iinclude -D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
CFLAGS += -Wall -std=gnu99 -g -Wno-deprecated-declarations -Wno-parentheses \
	-Wno-stringop-overflow -Wno-pointer-sign -Wno-incompatible-pointer-types \
	-I$(REPO_ROOT)/include -I$(BDIR)
LDFLAGS += $(LIBCRYPTO) $(LIBFTDI) $(LIBUSB)

PROGS:= $(BDIR)/shaft
SRCS  = $(BDIR)/args.o
SRCS += $(BDIR)/bootcon.o
SRCS += $(BDIR)/endorsement.o
SRCS += $(BDIR)/header.o
SRCS += $(BDIR)/mpsse.o
SRCS += $(BDIR)/nvmem.o
SRCS += $(BDIR)/rescue.o
SRCS += $(BDIR)/shaft.o
SRCS += $(BDIR)/spiflash.o
SRCS += $(BDIR)/support.o
SRCS += $(BDIR)/transfer.o

# Use VERBOSE=1 for debug output
ifeq ($(VERBOSE),)
Q := @
SUPPRESS := >/dev/null
else
Q :=
SUPPRESS := --verbose
endif

all: $(BDIR) $(PROGS)

.PHONY: clean
clean:
	$(Q)$(RM) -rf build

$(BDIR):
	$(Q)$(MKDIR) -p $@

$(BDIR)/version.h:
	$(Q)$(SHELL) $(REPO_ROOT)/util/version.sh $@

$(BDIR)/%.o: src/%.c
	@echo "  CC      $(notdir $<)"
	$(Q)$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(BDIR)/%.o: src/%.cc
	@echo "  CXX     $(notdir $<)"
	$(Q)$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

$(PROGS): $(BDIR)/version.h $(SRCS)
	@echo "  LD      $(notdir $@)"
	$(Q)$(LD) $^ -o $@ $(LDFLAGS)