# repo_root is passed from main Makefile
include $(REPO_ROOT)/toolchain.mk
include $(REPO_ROOT)/sources.mk

BDIR ?= build/$(ARCH)

# These are the flags used to compile normal binaries throughout src/*.c
CPPFLAGS += -Iinclude -D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE

CFLAGS += -Wall -std=gnu99 -g -Wno-deprecated-declarations -Wno-parentheses \
	-Wno-stringop-overflow -Wno-pointer-sign -Wno-incompatible-pointer-types \
	-Wno-unused-const-variable -I$(REPO_ROOT)/include -I$(BDIR) -Wno-unused-value

CXXFLAGS += -Wall -g -Wno-deprecated-declarations -Wno-parentheses \
	-Wno-stringop-overflow -I$(REPO_ROOT)/include -I$(BDIR) -Wno-unused-value

LDFLAGS += $(LIBUSB) $(LIBCRYPTO)

PROGS:= $(BDIR)/smiko
SRCS  = $(BDIR)/args.o
SRCS += $(BDIR)/board_id.o
SRCS += $(BDIR)/bn.o
SRCS += $(BDIR)/console.o
SRCS += $(BDIR)/nuggets.o
SRCS += $(BDIR)/smiko.o
SRCS += $(BDIR)/sysinfo.o
SRCS += $(BDIR)/tpm.o
SRCS += $(BDIR)/trunks.o
SRCS += $(BDIR)/usb.o
SRCS += $(BDIR)/vendor_cmds.o
SRCS += $(BDIR)/verify.o

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