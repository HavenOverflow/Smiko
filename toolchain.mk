# For cross-archtiecture compilation, set these environment variables to something like
# "aarch64" and "arm-none-eabi-" for arm64, if you're building FOR x86, then "x86_64" and "x86_64-linux-gnu-"
ARCH ?= $(shell uname -m)
CROSS_COMPILE ?= 
UNAME_S := $(shell uname -s)
GENDIR ?= build/gen
BINDIR ?= build/bin/$(ARCH)
LIBDIR ?= build/lib
RBDIR ?= build/rust/release

ifeq ($(CROSS_COMPILE),)
ifneq ($(ARCH),$(shell uname -m))

ifeq ($(ARCH),x86_64)
CROSS_COMPILE := x86_64-linux-gnu-
HEADER_DIR := $(abspath lib/x86_64/headers)
endif # ifeq ARCH,x86_64

ifeq ($(ARCH),aarch64)
CROSS_COMPILE := aarch64-linux-gnu-
HEADER_DIR := $(abspath lib/aarch64/headers)
endif # ifeq ARCH,aarch64

ifeq ($(ARCH),armv7l)
CROSS_COMPILE := arm-none-eabi-
HEADER_DIR := $(abspath lib/armv7l/headers)
endif# ifeq ARCH,armv7l

endif # ifneq ARCH,uname -m
endif # ifeq CROSS_COMPILE,

# we also want HEADER_DIR if we're statically compiling
ifneq ($(STATIC),)
ifndef HEADER_DIR
HEADER_DIR := $(abspath lib/$(ARCH)/headers)
endif
endif

### Standard build utilities ###
MAKE ?= make
SHELL := /bin/bash
PYTHON ?= python3
PROTOC ?= protoc
PKG_CONFIG ?= pkg-config
INSTALL ?= install

# -- macOS specific --
# if we have homebrew gcc, use it instead of apple clang.
# openssl and libusb aren't linked by default by homebrew too.
# expose keg pkg-config paths.
ifeq ($(UNAME_S),Darwin)
BREW_PREFIX    := $(shell brew --prefix 2>/dev/null || echo /opt/homebrew)
export PKG_CONFIG_PATH := $(BREW_PREFIX)/opt/openssl/lib/pkgconfig:$(PKG_CONFIG_PATH)
export PKG_CONFIG_PATH := $(BREW_PREFIX)/opt/libusb/lib/pkgconfig:$(PKG_CONFIG_PATH)
export PKG_CONFIG_PATH := $(BREW_PREFIX)/lib/pkgconfig:$(PKG_CONFIG_PATH)
_BREW_GCC      := $(shell ls $(BREW_PREFIX)/bin/gcc-[0-9]* 2>/dev/null | sort -V | tail -1)
ifneq ($(_BREW_GCC),)
CC  := $(_BREW_GCC)
CXX := $(subst gcc,g++,$(_BREW_GCC))
LD  := $(_BREW_GCC)
else
CC  := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
LD  := $(CROSS_COMPILE)gcc
endif # _BREW_GCC
else
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)gcc
endif # Darwin

### Chromium extension build utilities ###
CHROMIUM ?= $(firstword $(wildcard /usr/bin/google-chrome-stable \
			/usr/bin/google-chrome /usr/bin/chromium)) \
			--allow-legacy-extension-manifests

### Standard GNU/Linux Utilities ###
CP ?= cp
RM ?= rm
MKDIR ?= mkdir
TOUCH ?= touch
UNZIP ?= unzip
ZIP ?= zip

# maybe not all of these are needed, but it's easier to pass them all
MAKE_ARGS := \
    ARCH=$(ARCH) \
    CROSS_COMPILE=$(CROSS_COMPILE) \
    VERBOSE=$(VERBOSE) \
    STATIC=$(STATIC) \
    CC=$(CC) \
    CXX=$(CXX) \
    LD=$(LD) \
    PROTOC=$(PROTOC) \
    PKG_CONFIG=$(PKG_CONFIG) \
    HEADER_DIR=$(HEADER_DIR)