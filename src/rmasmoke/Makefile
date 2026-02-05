REPO_ROOT ?= ../..

include $(REPO_ROOT)/toolchain.mk
include $(REPO_ROOT)/sources.mk

BDIR ?= build/$(ARCH)

# These are the flags used to compile normal binaries throughout src/*.cc
CPPFLAGS += -Iinclude -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE \
		-I$(BDIR)/gen/proto -I$(BDIR)/gen -I$(REPO_ROOT)/third_party/nanopb \
		-I$(REPO_ROOT)/include -I$(BDIR)

CFLAGS += -Wall -std=gnu99 -g -Wno-deprecated-declarations -Wno-parentheses \
	-Wno-pointer-sign -Wno-incompatible-pointer-types -Wno-unused-value \
	-Wno-unused-const-variable
CXXFLAGS += -Wall -g -Wno-deprecated-declarations -Wno-parentheses -Wno-unused-value  \
			-Wno-sign-compare

ifndef USE_CLANG
CFLAGS += -Wno-stringop-overflow -Wno-maybe-uninitialized
CXXFLAGS += -Wno-stringop-overflow -Wno-maybe-uninitialized -Wno-write-strings
else
CFLAGS += -Wno-uninitialized
CXXFLAGS += -Wno-uninitialized -Wno-writable-strings
endif

LDFLAGS += $(LIBCPP) $(LIBCRYPTO) $(LIBTSS2)

PROGS:= $(BDIR)/rmasmoke
SRCS  = $(BDIR)/gen/proto/tpm_manager.pb.c
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
SUPPRESS := >/dev/null 2>/dev/null
else
Q :=
SUPPRESS :=
endif

all: $(BDIR) $(BDIR)/version.h $(PROGS)

.PHONY: clean
clean:
	$(Q)$(RM) -rf build

$(BDIR):
	$(Q)$(MKDIR) -p $@

$(BDIR)/version.h:
	$(Q)$(SHELL) $(REPO_ROOT)/util/version.sh $@

# There are not enough words to describe how sick of libprotbuf's abseil C++
# dependency, so i'm switching to NanoPB :3
$(BDIR)/gen/proto/%.pb.c: proto/%.proto
	@echo "  PROTOC  $(notdir $<)"
	$(Q)$(MKDIR) -p $(BDIR)/gen/proto
	$(Q)$(REPO_ROOT)/third_party/nanopb/generator/nanopb_generator $< \
		--output-dir=$(BDIR)/gen $(SUPPRESS)

$(BDIR)/%.o: src/%.c
	@echo "  CC      $(notdir $<)"
	$(Q)$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
$(BDIR)/%.o: $(REPO_ROOT)/third_party/nanopb/%.c
	@echo "  CC      $(notdir $<)"
	$(Q)$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(BDIR)/%.o: src/%.cc
	@echo "  CXX     $(notdir $<)"
	$(Q)$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

$(BDIR)/rmasmoke: $(SRCS)
	@echo "  LD      $(notdir $@)"
	$(Q)$(LD) $^ -o $@ $(LDFLAGS) $(CPPFLAGS)