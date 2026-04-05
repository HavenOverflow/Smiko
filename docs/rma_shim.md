# RMA Factory Repair Shims
A ChromeOS RMA Shim is a utility used by repair stations and manufacturers to facilitate in the diagnosing and testing of hardware problems with a Chromebook. These utilities are designed to be kept confidential, but were leaked in mass numbers as a result of the [Sh1mmer](https://github.com/MercuryWorkshop/Sh1mmer) vulnerability. This vulnerability allows for a Root Filesystem to be populated with attacker controlled payloads and directly executed without any form of DM-Verity performed by the Linux Kernel. Smiko takes advantage of this flaw and supports copying its utilities to RMA Shim images to run on supported ChromeOS machines.

First thing's first, if you don't already have one, go get an RMA Shim. Due to legal troubles from Google in the past, HavenOverflow will not directly provide information on how to obtain RMA Shims, but feel free to ask around in the [Crosbreaker discord server](https://discord.com/invite/crosbreaker-1375357349425971231). Once you've got an RMA Shim image, move it to the root of this repository and invoke the builder and image patcher as follows:

```bash
make clean
STATIC=1 make all tests
sudo bash patcher.sh -i /path/to/rma_shim.bin -l
```

From here, write the patched RMA Shim image to a USB device. You can also flash an already patched RMA Shim (e.g. with Sh1mmer) to a USB, and pass `--update` to the builder and `-i /dev/[insert USB device block here]` to update existing payloads.


