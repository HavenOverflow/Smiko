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
LDFLAGS += $(LIBCRYPTO)

PROGS:= $(BDIR)/carver
SRCS  = args.c
SRCS += carver.c
OBJS := $(addprefix $(BDIR)/, $(SRCS:.c=.o))

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

$(PROGS): $(BDIR)/version.h $(OBJS)
	@echo "  LD      $(notdir $@)"
	$(Q)$(LD) $^ -o $@ $(LDFLAGS)