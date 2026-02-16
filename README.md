# Smiko
Smiko - Smite Management by hIjacKing the cr50

![Smiko Logo](/logo.png)

<sub>(No longer for smiting management)</sub>

## Overview
Smiko is a collection of reverse engineered software and tools regarding the Google Security Chip. The tools in this repository are designed to interact with and in some cases exploit Google's lineup of chips to perform custom actions, extract data, and more.

The Google Security Chip (TM) is Google's proprietary off-shelf SoC that runs alongside the main processor in millions of licensed products (primarily Chromebooks, Google Pixel Phones, and Google's physical servers), serving a suite of hardware and software protection mechanisms to enable maximum control of the device on the hardware level whilst also preventing malicious tampering. Smiko aims to open-source this technology for security researchers to be able to understand and work with, and lead to overall improvement of this technology as time advances.

You can see a writeup of Smiko on [the website](https://smiko.havenoverflow.dev).

## Utilities Usage
There are many ways by which Smiko (and all software contained in this repo) can be utilized, of which here are the most straight-forward:
- [RMA Shim](/docs/rma_shim.md) (Safest, Recommended): Use Smiko or [RMASmoke](https://github.com/fWNavy/RMASmoke) using the TPM link existing on a modified RMA shim running in recovery mode.
- [Recovery Image](/docs/reco_image.md) ~~(Universal): Use Smiko or [RMASmoke](https://github.com/fWNavy/RMASmoke) using the TPM link existing on a patched Recovery Image.~~ (This will be replaced by Recomm3r, a project by crosbreaker that provides a GUI to modified recovery images)
- [SuzyQ](/docs/suzyq.md) (Universal, Recommended): Use Smiko using a GSC Debug Accessory and a piece of software.
- [Servo/C2D2](/docs/servo.md) (Universal): Use Smiko or Shaft using a Servo/C2D2 hardware header (Requires soldering skill!)
- [Developer Mode](/docs/devmode.md): Use Smiko or [RMASmoke](https://github.com/fWNavy/RMASmoke) using the command line on a Chromebook in Developer Mode (Cannot be done whilst enrolled _unless_ you're running [Shimboot](https://github.com/ading2210/shimboot) or a similar alternative).

All of the above installation means provide a universal method of using Smiko's tools on virtually any device.

More information can be found in the [docs folder](/docs), it is highly recommended you read all of them before continuing so you know what to expect and what your best option is.

## Reverse-Engineered Software
Smiko also contains decompilations of a lot of software that runs on the Google Security Chip lineup. Here is the full list of decompiled code, as well as paths to find them:
- [H1 BootROM](https://github.com/HavenOverflow/Cr50/tree/main/chip/haven/rom): Extracted and Decompiled copy of the Haven GSC BootROM (leaked via [RMASmoke](https://github.com/fWNavy/RMASmoke))
- [Cr50 RO](https://github.com/HavenOverflow/Cr50/tree/main/chip/haven/loader): Carved and Decompiled copy of the Cr50 RO firmware (NOT the same as chip/g/loader in chromium's source code)
- [Ti50](https://github.com/HavenOverflow/Ti50): Full decompilation of Ti50 for both Dauntless and OpenTitan, as well as custom implementations for Haven and Host-Emulation
- [Nugget-OS](https://github.com/HavenOverflow/nugget-os): Decompilation of Nugget-OS for Citadel and by extension the Acropora RTOS for Dauntless
- [Shaft](https://github.com/HavenOverflow/Smiko/tree/main/src/shaft): Custom reimplementation of the rescue and spiflash utilities from the closed-source cr50-utils repository (see docs for more info)

## Release Timeline
Smiko's full open-sourcing will be split up into 3 major releases, to allow development to continue without delaying further updates:
- Release 1: **Cr50 Loader**: Release all smiko utilities and the H1 BootROM and RO source code + binaries
- Release 2: **Nugget-OS**: Release updated smiko utilities and the C2 BootROM, RO, and Nugget-OS source code + binaries, Release D3 Acropora RO and RW source code + binaries
- Release 3: **Ti50**: Release updated smiko utilities and the D3 BootROM, RO, and Ti50 source code + binaries

This is currently Release 1, and thus you may run in to errors when attempting to clone the Ti50 and Nugget-OS repositories. Neither of these repositories are required to run `make all`. Releases 2 and 3 are in progress, you can join the discord public server (placed below) to stay posted.

Note: To all HOv staff members, please do not publish the Nugget-OS or Ti50 releases until properly completed.

## Copyright and Legal Notice
All of the code inside _this_ repo is the full property of HavenOverflow, unless otherwise stated by a copyright header in the top of the given source file. Redistribution of the contents of this repo is permissible so long as appropriate credit is provided and is not redistributed for monetary value. All contents of this repo are provided as is, free of charge, no negotiations.

HavenOverflow is **not responsible for any misuse of this software or anything that may spawn from it.** The only prebuilts _officially_ provided by HavenOverflow is the utilities, any other public redistributions of software contained in this repo is unofficial by a third party.

**All decompiled software** by HavenOverflow contains an appropriate copyright header at the top of each source code file depicting the year (estimated) it was written, as well as the original authors of the reverse engineered code. HavenOverflow and its affiliates claim _no ownership_ or copyright over reverse engineered software, and have provided all distributed code for educational purposes exclusively. No monetary value of any form has been accepted or recieved as a result of this code.

More information on the [Code Of Conduct](CODE_OF_CONDUCT.md) and [Security](SECURITY.md) for this repository can be found in their respective documents. For any potential legal concerns, please email legal@havenoverflow.dev.

## Credits and Special Thanks

Core Contributors
- [Hannah](https://github.com/ZegLolTheThirtySixth): Pioneering development, building the payloads and injectors
- [Appleflyer](https://github.com/appleflyerv3): Completing the H1 BootROM dump, major help with development and research, lead GSCEmulator dev
- [Evelyn](https://github.com/Evenlyn3440): Major help decompiling and developing, soldering to test Shaft
- [Kayla](https://github.com/TheSpiritOfTheDark): Helping compile & decompile things, AP and EC hardware insight
- [WeirdTreeThing](https://github.com/WeirdTreeThing): Soldering to test shaft, hardware insight, helping compile things
- [OlyB](https://github.com/BinBashBanana): Insight into the ChromeOS and GSC ecosystems
- [Leah](https://github.com/OtterCodes101): Helping compile things and testing
- [Writable](https://github.com/MunyDev): Creating RMASmoke which allowed us to dump the H1's BootROM

***!-!-!-! People below this are not HavenOverflow members and may not be affiliated with us !-!-!-!***

- [Kxtz](https://github.com/kxtzownsu): Fixing various bugs and misc development, testing
- [Quarkslab](https://quarkslab.com): Proving the concept possible and introducing us to Citadel and NuggetOS
- [Google](https://github.com/Google): Developing the GSC and all provided firmware

## Discord
- [Join HavenOverflow](https://discord.gg/BtJ6j2eJrU)
- [Join Crosbreaker](https://discord.crosbreaker.dev)
