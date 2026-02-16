#!/usr/bin/env bash
TARGET_ARCH="${1}"
HOST_ARCH="$(uname -m)"

. /etc/os-release
case "$ID" in
  debian|ubuntu|linuxmint|pop)
    ;;
  *)
    exit 0
    ;;
esac

if [ -e .${TARGET_ARCH}_requirements_met ]; then
    exit 0
fi

missing=0

echo "Checking dependencies for arch: $TARGET_ARCH"

cmds=(
  git
  make
  protoc
  arm-none-eabi-gcc
  python3
)

if [ "$TARGET_ARCH" != "$HOST_ARCH" ]; then
  case "$ARCH" in
    aarch64)
      cmds+=(aarch64-linux-gnu-gcc aarch64-linux-gnu-g++)
      ;;
    x86_64)
      cmds+=(x86_64-linux-gnu-gcc x86_64-linux-gnu-g++)
      ;;
    armv7l)
      cmds+=(arm-none-eabi-gcc arm-none-eabi-g++)
      ;;
    *)
      echo "unsupported target arch: $TARGET_ARCH"
      exit 1
      ;;
  esac
else 
  cmds+=(gcc g++)
fi

# I'm not sure if libftdi-dev AND libftdi1-dev are both needed, but it works fine with both so whatever
pkgs=(
  libtss2-dev
  libusb-1.0-0-dev
  libftdi1-dev
  libftdi-dev
  libelf-dev
  libxml2-dev
  rapidjson-dev
)

py_pkgs=(
  protobuf
  grpcio-tools
)

for c in "${cmds[@]}"; do
  if ! command -v "$c" >/dev/null 2>&1; then
    echo "missing command: $c"
    missing=1
  fi
done

for p in "${pkgs[@]}"; do
  if ! dpkg -s "$p" >/dev/null 2>&1; then
    echo "missing package: $p"
    missing=1
  fi
done

if ! command -v pip3 >/dev/null 2>&1; then
  echo "pip3 not installed"
  missing=1
else
  for p in "${py_pkgs[@]}"; do
    if ! pip3 show "$p" >/dev/null 2>&1; then
      echo "missing pip package: $p"
      missing=1
    fi
  done
fi

if [ "$missing" -ne 0 ]; then
  echo "Check docs/building.md to install required packages."
  exit 1
else
  touch .${TARGET_ARCH}_requirements_met #because requirement scan is slow
fi