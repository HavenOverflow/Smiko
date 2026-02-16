include $(REPO_ROOT)/toolchain.mk
include $(REPO_ROOT)/sources.mk

ifeq ($(ARCH),armv7l)
CFLAGS += -mthumb -mcpu=cortex-m3 -march=armv7-m
endif

# Use VERBOSE=1 for debug output
ifeq ($(VERBOSE),)
Q := @
else
Q :=
endif

CFLAGS += -Wno-deprecated-declarations
INCLUDES += -Iinclude -I$(REPO_ROOT)/src/tpm2

BDIR := build/$(ARCH)
ODIR := $(REPO_ROOT)/build/bin/$(ARCH)
TPM2 := $(REPO_ROOT)/src/tpm2
TPM2_BDIR := $(TPM2)/build/$(ARCH)

SOURCES += vendor-code-string.c
SOURCES += tpm2-simulator.c

OBJS := $(addprefix $(BDIR)/, $(SOURCES:.c=.o))

all: $(BDIR) $(ODIR)/tpm2-simulator 

$(ODIR):
	$(Q)$(MKDIR) -p $(ODIR)
$(TPM2_BDIR):
	$(Q)$(MKDIR) -p $(TPM2_BDIR)
$(BDIR): $(TPM2_BDIR)
	$(Q)$(MKDIR) -p $(BDIR)

$(TPM2)/build/$(ARCH)/libtpm2.a:
	@echo "  MAKE    tpm2"
	$(Q)$(MAKE) CFLAGS="$(CFLAGS) $(INCLUDES)" obj=$(TPM2_BDIR) -C $(TPM2)

$(BDIR)/%.o: %.c
	@echo "  CC      $(notdir $<)"
	$(Q)$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

$(ODIR)/tpm2-simulator: $(OBJS) $(TPM2)/build/$(ARCH)/libtpm2.a
	@echo "  LD      $(notdir $@)"
	$(Q)$(LD) $^ $(LIBS) -o $@ $(CFLAGS) $(INCLUDES) $(LIBCRYPTO)

clean:
	rm -rf $(ODIR)/tpm2-simulator
