# repo_root is passed from main Makefile
include $(REPO_ROOT)/toolchain.mk
include $(REPO_ROOT)/sources.mk

BDIR ?= build/$(ARCH)

# These are the flags used to compile normal binaries throughout src/*.cc
CPPFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-D_LARGEFILE64_SOURCE -Iinclude  -I$(REPO_ROOT)/include -I$(BDIR) \
	-I$(BDIR)/gen -I$(BDIR)/gen/src -Ithird_party/nanopb
CFLAGS += -Wall -g -Wno-deprecated-declarations -Wno-parentheses \
	-Wno-stringop-overflow -Wno-sign-compare -Wno-format-security \
	-Wno-maybe-uninitialized
CXXFLAGS += -Wall -g -Wno-deprecated-declarations -Wno-parentheses \
	-Wno-stringop-overflow -Wno-unused-value -Wno-sign-compare \
	-Wno-maybe-uninitialized

LDFLAGS += $(LIBCPP) $(LIBCRYPTO) $(LIBTSS2)

PROGS:= $(BDIR)/rmasmoke
SRCS  = $(BDIR)/gen/src/tpm_manager.pb.c
SRCS += $(BDIR)/args.o
SRCS += $(BDIR)/nvmem.o
SRCS += $(BDIR)/pb_common.o
SRCS += $(BDIR)/pb_decode.o
SRCS += $(BDIR)/pb_encode.o
SRCS += $(BDIR)/rmasmoke.o
SRCS += $(BDIR)/rop.o

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

# There are not enough words to describe how sick of libprotbuf's abseil C++
# dependency, so i'm switching to NanoPB :3
$(BDIR)/gen/src/tpm_manager.pb.c: src/tpm_manager.proto
	@echo "  PROTOC  $(notdir $<)"
	$(Q)$(MKDIR) -p $(BDIR)/gen/src
	$(Q)third_party/nanopb/generator/nanopb_generator $< --output-dir=$(BDIR)/gen 2>/dev/null

$(BDIR)/%.o: src/%.c
	@echo "  CC      $(notdir $<)"
	$(Q)$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
$(BDIR)/%.o: third_party/nanopb/%.c
	@echo "  CC      $(notdir $<)"
	$(Q)$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(BDIR)/%.o: src/%.cc
	@echo "  CXX     $(notdir $<)"
	$(Q)$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

$(BDIR)/rmasmoke: $(BDIR)/version.h $(SRCS)
	@echo "  LD      $(notdir $@)"
	$(Q)$(LD) $^ -o $@ $(LDFLAGS) $(CPPFLAGS)