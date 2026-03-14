 Arduino Single Pair Ethernet (10BASE-T1S) Library
====================
[![Compile Examples](https://github.com/bcmi-labs/Arduino_10BASE_T1S/workflows/Compile%20Examples/badge.svg)](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions?workflow=Compile+Examples)
[![Spell Check status](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions/workflows/spell-check-task.yml/badge.svg)](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions/workflows/spell-check-task.yml)
[![Sync Labels status](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions/workflows/sync-labels.yml/badge.svg)](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions/workflows/sync-labels.yml)
[![Arduino Lint](https://github.com/bcmi-labs/Arduino_10BASE_T1S/workflows/Arduino%20Lint/badge.svg)](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions?workflow=Arduino+Lint)

This library implements the **10BASE-T1S** standard (Single Pair Ethernet) for selected Arduino boards.  

It enables low-speed Ethernet connectivity over a single twisted pair, ideal for industrial and automotive applications.

### What is 10BASE-T1S?
10BASE‑T1S is an Ethernet physical layer standard defined by IEEE (part of IEEE 802.3cg) that enables Ethernet communication over a single twisted pair cable at speeds up to 10 Mbps.

## 📖 Documentation
For more information on the features of this library and how to use them please read the documentation [here](./docs/).

## ✅ Supported Boards

This library was tested and validated on the following Arduino boards:
- [Arduino Zero](https://store.arduino.cc/products/arduino-zero)
- [Arduino UNO R4 WiFi](https://store.arduino.cc/products/uno-r4-wifi)
- [Arduino UNO R4 Minima](https://store.arduino.cc/products/uno-r4-minima)
- RP2040 boards (for example Raspberry Pi Pico / Pico 2040) using the Arduino-Pico core

> [!IMPORTANT]
> These boards do not include a native 10BASE‑T1S PHY or transceiver.
To enable Single Pair Ethernet communication, you must connect an external and compatible 10BASE‑T1S transceiver to your board via SPI.

> [!NOTE]
> The library may work on other boards, but they are not officially supported.

## RP2040 / Pico Notes

When building for RP2040 (Arduino-Pico), this library uses the lwIP stack provided by the board core.

- Source files switch behavior with `ARDUINO_ARCH_RP2040`.
- RP2040 builds include core headers such as `<lwip/netif.h>`.
- Bundled lwIP C sources are stored in `third_party/liblwip`.
- `src/lib/liblwip` keeps headers/config (`include`, `cfg`, `arch`) for compatibility with existing include paths.

In this repository, bundled lwIP C files are compiled through wrapper translation units in `src/lib/lwip_vendor`. Each wrapper is guarded with `#if !defined(ARDUINO_ARCH_RP2040)`.

This avoids duplicate/conflicting lwIP compilation on RP2040 while keeping bundled lwIP code available for other board ports.

## Build Layout (Arduino IDE + PlatformIO)

This repository now uses a single-tree layout that works in both Arduino IDE and PlatformIO without toolchain-specific source filtering.

Current structure:

```text
Arduino_10BASE_T1S/
	src/
		lib/
			liblwip/
				arch/
				cfg/
				include/
			lwip_vendor/
				lwip_vendor_*.c
	third_party/
		liblwip/
			api/
			core/
			netif/
			include/
			cfg/
			arch/
```

Wrapper file pattern:

```c
#if !defined(ARDUINO_ARCH_RP2040)
#include "../../../third_party/liblwip/core/netif.c"
#endif
```

Why this works in both IDEs:

- Arduino IDE compiles only what is under `src/`, so RP2040 can skip bundled lwIP at source level.
- PlatformIO compiles the same wrapper files, so behavior stays consistent.
- RP2040 uses core lwIP while non-RP2040 builds can compile bundled lwIP sources.

## 📕 Further Reading
- [Arduino UNO SPE Shield](https://docs.arduino.cc/hardware/spe-shield/)

## 🐛 Reporting Issues

If you encounter any issue, please open a bug report [here](https://github.com/arduino-libraries/Arduino_10BASE_T1S/issues). 

For questions, comments, or feedback on this library, please use the official [Arduino Forum](https://forum.arduino.cc/).

## 🫂 Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## ⚖️ License

This library is released under the [MPL-2.0 license](http://mozilla.org/MPL/2.0/).
