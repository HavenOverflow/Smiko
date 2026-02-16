# Copyright 2025 HavenOverflow
# Use of this code is permissible so long as the appropriate credit
# is provided. See the LICENSE file for more info.

include toolchain.mk

MAKE_ARGS += REPO_ROOT=$(abspath .)

# Build variables
BDIR ?= build
# TODO (Hannah): Add NOSClient in release wave 2.
PROGS ?= smiko shaft carver gsctool rmasmoke # nosclient
FIRMWARE ?= smiko-cfw # cr50 ti50 nugget-os
TESTS ?= tpm2-simulator codesigner # Keep cr50-codesigner here until we have the static deps ~ Hannah
CLEAN_DIRS := $(BDIR) src/smiko/build src/shaft/build \
	src/carver/build src/rmasmoke/build src/cr50/build src/tpm2/build \
	src/ti50/common/build src/ti50/third_party/tock/tock/target src/nugget-os/build \
	test/signer/build test/tpm2-simulator/build

SUBMODULE_DIRS := \
	src/cr50 \
	src/tpm2 \
	third_party/nanopb

# SUBMODULE_DIRS += src/ti50 src/nugget-os test/gscemulator

# Use VERBOSE=1 for debug output
ifeq ($(VERBOSE),)
Q := @
SUPPRESS := >/dev/null
else
Q :=
SUPPRESS := --verbose
endif

# Shorthands
all: utils firmware
all-tests: all tests
utils: check-deps $(PROGS)
firmware: check-deps $(FIRMWARE)
tests: check-deps $(TESTS)

.PHONY: check-deps
check-deps:
	@for dir in $(SUBMODULE_DIRS); do \
		if [ ! -d $$dir ] || [ -z "$$(find $$dir -mindepth 1 -print -quit 2>/dev/null)" ]; then \
			echo "Getting submodules" >&2; \
			git submodule init; \
			git submodule update; \
			break; \
		fi \
	done

	$(Q)$(SHELL) ./requirements.sh $(ARCH)

.PHONY: clean
clean:
	$(Q)$(RM) -rf $(CLEAN_DIRS)

.PHONY: install
install:
	@echo "  INSTALL build"
	$(Q)$(INSTALL) -d /usr/local/bin/
	$(Q)$(INSTALL) -m +x $(BINDIR)/* /usr/local/bin/

	@if ls build/lib/$(ARCH)/* >/dev/null 2>&1; then \
		$(INSTALL:@%=install) -d /usr/local/lib; \
		$(INSTALL:@%=install) -m -x build/lib/$(ARCH)/* /usr/local/lib/; \
	fi

	$(Q)$(INSTALL) -d /usr/share/smiko/firmware
	$(Q)$(INSTALL) -m -x build/firmware/* /usr/share/smiko/firmware
	$(Q)$(INSTALL) -d /usr/share/smiko/include
	$(Q)$(INSTALL) -m -x build/gen/include/* /usr/share/smiko/include

.PHONY: help
help:
	@echo "Smiko Builder"
	@echo "Common Targets:"
	@echo "  all                  - Build all available programs"
	@echo "  utils                - Build all command line utilities"
	@echo "  firmware             - Build all firmware"
	@echo "  tests                - Build all test programs"
	@echo "  install              - Install built programs onto your machine"
	@echo "  clean                - Remove all build files"
	@echo ""
	@echo "Buildable Utilities:"
	@echo "  smiko                - Smiko command line utility and firmware injector"
	@echo "  shaft                - Shaft command line utility and firmware injector"
	@echo "  rmasmoke             - RMASmoke command line utility and TPM2 overflow manipulator"
	@echo "  carver               - Cr50 firmware payload extractor"
	@echo "  gsctool              - GSC firmware updater and debugger"
	@echo "  codesigner           - GSC firmware header and code signer"
	@echo ""
	@echo "Buildable Tests:"
#   @echo "  tock-on-titan        - Port of TockOS to the H1" # TODO (Hannah): Do we need this?
#   @echo "  gscemulator          - GSC hardware emulator" # TODO (Hannah): Compile with Cpython?
	@echo "  tpm2-simulator       - TPM2 software simulator"
	@echo ""
	@echo "Buildable Firmware:"
	@echo "  cr50                 - Cr50 Stock GSC firmware"
	@echo ""
	@echo "Environment Variables:"
	@echo "  VERBOSE=1            - Show all commands executed when building"
	@echo "  STATIC=1             - Compile all programs statically"
	@echo "  CROSS_COMPILE=       - Override the compiler type for all programs"
	@echo "  ARCH=x86_64          - The architecture for the target device"
	@echo ""
	@echo "Examples:"
	@echo "  make STATIC=0 ARCH=x86_64 all tests"
	@echo "  make STATIC=1 ARCH=aarch64 utils firmware"
	@echo "  make STATIC=1 ARCH=armv7l tests"


# Build directories
$(BDIR)-$(ARCH):
	$(Q)$(MKDIR) -p $(BDIR)/bin/$(ARCH) $(BDIR)/lib/$(ARCH) \
		$(BDIR)/firmware $(BDIR)/gsctool/$(ARCH)


# Utilities
smiko: check-deps $(BDIR)-$(ARCH) $(BDIR)/bin/$(ARCH)/smiko
$(BDIR)/bin/$(ARCH)/smiko:
	$(Q)$(MAKE) $(MAKE_ARGS) -C src/smiko --no-print-directory
	$(Q)$(CP) src/smiko/build/$(ARCH)/smiko $@

shaft: check-deps $(BDIR)-$(ARCH) $(BDIR)/bin/$(ARCH)/shaft
$(BDIR)/bin/$(ARCH)/shaft:
	$(Q)$(MAKE) $(MAKE_ARGS) -C src/shaft --no-print-directory
	$(Q)$(CP) src/shaft/build/$(ARCH)/shaft $@

carver: check-deps $(BDIR)-$(ARCH) $(BDIR)/bin/$(ARCH)/carver
$(BDIR)/bin/$(ARCH)/carver:
	$(Q)$(MAKE) $(MAKE_ARGS) -C src/carver --no-print-directory
	$(Q)$(CP) src/carver/build/$(ARCH)/carver $@

gsctool: check-deps $(BDIR)-$(ARCH) $(BDIR)/bin/$(ARCH)/gsctool
$(BDIR)/bin/$(ARCH)/gsctool:
	$(Q)$(MAKE) $(MAKE_ARGS) -C src/cr50/extra/usb_updater --no-print-directory

rmasmoke: check-deps $(BDIR)-$(ARCH) $(BDIR)/bin/$(ARCH)/rmasmoke
$(BDIR)/bin/$(ARCH)/rmasmoke:
	$(Q)$(MAKE) $(MAKE_ARGS) -C src/rmasmoke --no-print-directory
	$(Q)$(CP) src/rmasmoke/build/$(ARCH)/rmasmoke $@

codesigner: check-deps $(BDIR)-$(ARCH) $(BDIR)/bin/$(ARCH)/cr50-codesigner
$(BDIR)/bin/$(ARCH)/cr50-codesigner:
	$(Q)$(MAKE) $(MAKE_ARGS) -C test/signer --no-print-directory
	$(Q)$(CP) test/signer/build/$(ARCH)/cr50-codesigner $@



# Firmware
smiko-cfw: check-deps $(BDIR)-$(ARCH) $(BDIR)/firmware/smiko-bootcon-cfw.bin \
	$(BDIR)/firmware/smiko-update-cfw.bin

src/cr50/build/smiko/RW/ec.RW.flat:
	$(Q)BRANCH=TOT BOARD=smiko $(MAKE) -C src/cr50 --no-print-directory

$(BDIR)/firmware/smiko-bootcon-cfw.bin: src/cr50/build/smiko/RW/ec.RW.flat
	$(Q)$(SHELL) util/packer.sh $@ \
		src/cr50/prebuilts/node_locked_images/cr50.prod.test.ro.A.0.0.11.bin \
		src/cr50/build/smiko/RW/ec.RW.flat \
		test/carver/extracted/cr50-0-0-9.ro.b.flat \
		test/carver/extracted/cr50-0-0-6.rw.b.flat \
		>/dev/null 2>/dev/null

$(BDIR)/firmware/smiko-update-cfw.bin: src/cr50/build/smiko/RW/ec.RW.flat
	$(Q)BRANCH=TOT BOARD=smiko $(MAKE) -C src/cr50 --no-print-directory
	$(Q)$(SHELL) util/packer.sh $@ \
		test/carver/extracted/cr50-0-0-9.ro.a.flat \
		src/cr50/build/smiko/RW/ec.RW.flat \
		test/carver/extracted/cr50-0-0-9.ro.b.flat \
		src/cr50/build/smiko/RW/ec.RW_B.flat \
		>/dev/null 2>/dev/null

cr50: $(BDIR) $(BDIR)/firmware/cr50.bin
$(BDIR)/firmware/cr50.bin: $(BDIR)/bin/$(ARCH)/cr50-codesigner
	$(Q)BRANCH=TOT BOARD=cr50 $(MAKE) -C src/cr50 --no-print-directory
	$(Q)$(CP) src/cr50/build/cr50/ec.bin $@

# Tests
tpm2-simulator: $(BDIR) $(BDIR)/bin/$(ARCH)/tpm2-simulator
$(BDIR)/bin/$(ARCH)/tpm2-simulator:
	$(Q)$(MAKE) $(MAKE_ARGS) -C test/tpm2-simulator --no-print-directory
	$(Q)$(CP) src/tpm2/build/$(ARCH)/libtpm2.a $(BDIR)/lib/$(ARCH)/libtpm2.a
	
