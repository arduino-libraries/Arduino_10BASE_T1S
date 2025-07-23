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

> [!IMPORTANT]
> These boards do not include a native 10BASE‑T1S PHY or transceiver.
To enable Single Pair Ethernet communication, you must connect an external and compatible 10BASE‑T1S transceiver to your board via SPI.

> [!NOTE]
> The library may work on other boards, but they are not officially supported.

## 📕 Further Reading
- [Arduino UNO SPE Shield](https://docs.arduino.cc/hardware/spe-shield/)

## 🐛 Reporting Issues

If you encounter any issue, please open a bug report [here](https://github.com/arduino-libraries/Arduino_10BASE_T1S/issues). 

For questions, comments, or feedback on this library, please use the official [Arduino Forum](https://forum.arduino.cc/).

## 🫂 Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## ⚖️ License

This library is released under the [MPL-2.0 license](http://mozilla.org/MPL/2.0/).
