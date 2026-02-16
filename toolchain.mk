# For cross-archtiecture compilation, set these environment variables to something like
# "aarch64" and "arm-none-eabi-" for arm64, if you're building FOR x86, then "x86_64" and "x86_64-linux-gnu-"
ARCH ?= $(shell uname -m)
CROSS_COMPILE ?= 
GENDIR ?= build/gen
BINDIR ?= build/bin/$(ARCH)
LIBDIR ?= build/lib
RBDIR ?= build/rust/release

ifeq ($(CROSS_COMPILE),)
ifeq ($(ARCH),x86_64)
CROSS_COMPILE := x86_64-linux-gnu-
HEADER_DIR := $(abspath lib/x86_64/headers)
endif
ifeq ($(ARCH),aarch64)
CROSS_COMPILE := aarch64-linux-gnu-
HEADER_DIR := $(abspath lib/aarch64/headers)
endif
ifeq ($(ARCH),armv7l)
CROSS_COMPILE := arm-none-eabi-
HEADER_DIR := $(abspath lib/armv7l/headers)
endif
endif # ifeq CROSS_COMPILE,

### Standard build utilities ###
MAKE ?= make
SHELL := /bin/bash
PYTHON ?= python3
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)gcc
PROTOC ?= protoc
PKG_CONFIG ?= pkg-config
INSTALL ?= install

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