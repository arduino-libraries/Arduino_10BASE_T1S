# Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class ` [`Arduino_10BASE_T1S_PHY_Interface`](#class_arduino__10_b_a_s_e___t1_s___p_h_y___interface) | 
`class ` [`Arduino_10BASE_T1S_UDP`](#class_arduino__10_b_a_s_e___t1_s___u_d_p) | UDP communication class for Arduino 10BASE-T1S library.
`class ` [`MacAddress`](#class_mac_address) | 
`class ` [`T1SMacSettings`](#class_t1_s_mac_settings) | 
`class ` [`T1SPlcaSettings`](#class_t1_s_plca_settings) | 
`class ` [`Arduino_10BASE_T1S_UDP::UdpRxPacket`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet) | 

# class `Arduino_10BASE_T1S_PHY_Interface` <a id="class_arduino__10_b_a_s_e___t1_s___p_h_y___interface" class="anchor"></a>

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
| [`~Arduino_10BASE_T1S_PHY_Interface`](#class_arduino__10_b_a_s_e___t1_s___p_h_y___interface_1a283666612d51bdd1083cbd567f809313) |  |
| [`begin`](#class_arduino__10_b_a_s_e___t1_s___p_h_y___interface_1a5d34573236cf5301ff1fb7cb05b6360d) | Initializes the PHY interface with the specified network settings. |
| [`service`](#class_arduino__10_b_a_s_e___t1_s___p_h_y___interface_1a72b23b0beb34cdc979ea3043a7142ed4) |  |

## Members

### `~Arduino_10BASE_T1S_PHY_Interface` <a id="class_arduino__10_b_a_s_e___t1_s___p_h_y___interface_1a283666612d51bdd1083cbd567f809313" class="anchor"></a>

```cpp
inline virtual ~Arduino_10BASE_T1S_PHY_Interface()
```

<hr />

### `begin` <a id="class_arduino__10_b_a_s_e___t1_s___p_h_y___interface_1a5d34573236cf5301ff1fb7cb05b6360d" class="anchor"></a>

```cpp
bool begin(IPAddress const ip_addr, IPAddress const network_mask, IPAddress const gateway, MacAddress const mac_addr, T1SPlcaSettings const t1s_plca_settings, T1SMacSettings const t1s_mac_settings)
```

Initializes the PHY interface with the specified network settings.

This method configures the PHY interface with the provided IP address, network mask, gateway, MAC address, and PLCA settings.

#### Parameters
* `ip_addr` The IP address to assign to the interface. 

* `network_mask` The network mask to use. 

* `gateway` The gateway IP address. 

* `mac_addr` The MAC address to assign to the interface. 

* `t1s_plca_settings` The PLCA settings to use. 

#### Returns
Returns true if the initialization was successful, false otherwise.
<hr />

### `service` <a id="class_arduino__10_b_a_s_e___t1_s___p_h_y___interface_1a72b23b0beb34cdc979ea3043a7142ed4" class="anchor"></a>

```cpp
void service()
```

<hr />

# class `Arduino_10BASE_T1S_UDP` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p" class="anchor"></a>

```cpp
class Arduino_10BASE_T1S_UDP
  : public UDP
```

UDP communication class for Arduino 10BASE-T1S library.

This class provides an implementation of the UDP protocol for the Arduino 10BASE-T1S library. It enables sending and receiving UDP packets over a 10BASE-T1S Ethernet interface, supporting both transmission and reception of data. The class inherits from the standard Arduino UDP base class, and overrides its methods to provide the necessary functionality for packet management, buffer handling, and communication with the underlying hardware.

Features:

* Initialization and termination of UDP sockets.

* Sending UDP packets to specified IP addresses or hostnames and ports.

* Receiving UDP packets, with support for buffer management and packet queueing.

* Access to remote sender's IP address and port for received packets.

* Internal buffer size configuration for received packets.

This class is intended for use with the Arduino 10BASE-T1S library and is not a general-purpose UDP implementation.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
| [`Arduino_10BASE_T1S_UDP`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1ae0856b8ad00f22033db19558559bb594) |  |
| [`~Arduino_10BASE_T1S_UDP`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1aea7ab46c531587898331498288a843f5) |  |
| [`begin`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a3d3d71217d030db24729c39328d55467) | Initializes the UDP instance to listen on the specified port. |
| [`stop`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1aef6ec293190f8b50ea7b3a88f3368fb4) | Stops the UDP connection and releases any associated resources. |
| [`beginPacket`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a21b682176a40547e82b27c1ccfaac02d) | Begins the construction of a UDP packet to the specified IP address and port. |
| [`beginPacket`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1aa324c5998b19e20a727522e2f50c7699) | Begins the construction of a UDP packet to the specified host and port. |
| [`endPacket`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a877b4c91ef89d74f1ddd1041383d2ecb) | Finishes the construction of a UDP packet and sends it. |
| [`write`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1adff44e3087ff769ce9e07079f3bac8ce) | Sends a single byte of data in the current UDP packet. |
| [`write`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a951b69ca9da8cfe1863d3f2170312c0c) | Sends a buffer of data in the current UDP packet. |
| [`parsePacket`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1af7a1b610d6e941a4f75d71d5d837129a) | Sends a string in the current UDP packet. |
| [`available`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a6fda5e99522bf30bf3a0b6a1bcea374c) | Checks if there are any incoming UDP packets available to read. |
| [`read`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1ab0a1ea7854f2175caf3c662da8d65366) | Reads a single byte from the current UDP packet. |
| [`read`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1abcc0b7e38d5c3523e031eaebf946fc5c) | Reads a specified number of bytes from the current UDP packet into a buffer. |
| [`read`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a3e5de3919a838c551d083d000fa43551) | Reads a specified number of bytes from the current UDP packet into a character buffer. |
| [`peek`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a00c33af0714cb9286ca9c022614ecd37) | Peeks at the next byte in the current UDP packet without removing it from the buffer. |
| [`flush`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1aa90a09dd0de4548708561bea2f590db2) | Flushes the current UDP packet buffer. |
| [`remoteIP`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a1da65ecad341c2867e371992acd4610b) | Returns the IP address of the remote host that sent the last received packet. |
| [`remotePort`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a4137463ae8d926184532274a690733c3) | Returns the port number of the remote host that sent the last received packet. |
| [`onUdpRawRecv`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1a60b49bb4946021917c658ace0d56dd77) |  |
| [`bufferSize`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1af9fdf26f7aa66527b801f9371788b226) |  |

## Members

### `Arduino_10BASE_T1S_UDP` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1ae0856b8ad00f22033db19558559bb594" class="anchor"></a>

```cpp
Arduino_10BASE_T1S_UDP()
```

<hr />

### `~Arduino_10BASE_T1S_UDP` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1aea7ab46c531587898331498288a843f5" class="anchor"></a>

```cpp
virtual ~Arduino_10BASE_T1S_UDP()
```

<hr />

### `begin` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a3d3d71217d030db24729c39328d55467" class="anchor"></a>

```cpp
virtual uint8_t begin(uint16_t port)
```

Initializes the UDP instance to listen on the specified port.

This method sets up the UDP protocol to begin listening for incoming packets on the given port. It should be called before attempting to send or receive UDP packets.

#### Parameters
* `port` The local port to listen on. 

#### Returns
Returns 1 if successful, 0 if there was an error.
<hr />

### `stop` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1aef6ec293190f8b50ea7b3a88f3368fb4" class="anchor"></a>

```cpp
virtual void stop()
```

Stops the UDP connection and releases any associated resources.

This method overrides the base class implementation to properly close the UDP socket and perform necessary cleanup operations.
<hr />

### `beginPacket` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a21b682176a40547e82b27c1ccfaac02d" class="anchor"></a>

```cpp
virtual int beginPacket(IPAddress ip, uint16_t port)
```

Begins the construction of a UDP packet to the specified IP address and port.

Initializes the UDP packet buffer and prepares it to send data to the given destination.

#### Parameters
* `ip` The destination IP address. 

* `port` The destination port number. 

#### Returns
int Returns 1 on success, 0 on failure.
<hr />

### `beginPacket` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1aa324c5998b19e20a727522e2f50c7699" class="anchor"></a>

```cpp
virtual int beginPacket(const char * host, uint16_t port)
```

Begins the construction of a UDP packet to the specified host and port.

Initializes the UDP packet buffer and prepares it to send data to the given destination.

#### Parameters
* `host` The destination host name or IP address as a string. 

* `port` The destination port number. 

#### Returns
int Returns 1 on success, 0 on failure.
<hr />

### `endPacket` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a877b4c91ef89d74f1ddd1041383d2ecb" class="anchor"></a>

```cpp
virtual int endPacket()
```

Finishes the construction of a UDP packet and sends it.

This method finalizes the UDP packet and transmits it to the previously specified destination IP address and port.

#### Returns
int Returns 1 on success, 0 on failure.
<hr />

### `write` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1adff44e3087ff769ce9e07079f3bac8ce" class="anchor"></a>

```cpp
virtual size_t write(uint8_t data)
```

Sends a single byte of data in the current UDP packet.

This method appends a single byte to the current UDP packet buffer.

#### Parameters
* `data` The byte of data to send. 

#### Returns
size_t Returns the number of bytes written, which is always 1 for a single byte.
<hr />

### `write` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a951b69ca9da8cfe1863d3f2170312c0c" class="anchor"></a>

```cpp
virtual size_t write(const uint8_t * buffer, size_t size)
```

Sends a buffer of data in the current UDP packet.

This method appends a specified number of bytes from the provided buffer to the current UDP packet buffer.

#### Parameters
* `buffer` Pointer to the data buffer to send. 

* `size` The number of bytes to write from the buffer. 

#### Returns
size_t Returns the number of bytes written, which may be less than size if an error occurs.
<hr />

### `parsePacket` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1af7a1b610d6e941a4f75d71d5d837129a" class="anchor"></a>

```cpp
virtual int parsePacket()
```

Sends a string in the current UDP packet.

This method appends a null-terminated string to the current UDP packet buffer.

#### Parameters
* `str` Pointer to the null-terminated string to send. 

#### Returns
size_t Returns the number of bytes written, including the null terminator.
<hr />

### `available` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a6fda5e99522bf30bf3a0b6a1bcea374c" class="anchor"></a>

```cpp
virtual int available()
```

Checks if there are any incoming UDP packets available to read.

This method checks the internal buffer for any received UDP packets.

#### Returns
int Returns the number of available bytes in the current packet, or 0 if no packets are available.
<hr />

### `read` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1ab0a1ea7854f2175caf3c662da8d65366" class="anchor"></a>

```cpp
virtual int read()
```

Reads a single byte from the current UDP packet.

This method retrieves the next byte from the current UDP packet buffer.

#### Returns
int Returns the byte read, or -1 if no data is available.
<hr />

### `read` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1abcc0b7e38d5c3523e031eaebf946fc5c" class="anchor"></a>

```cpp
virtual int read(unsigned char * buffer, size_t len)
```

Reads a specified number of bytes from the current UDP packet into a buffer.

This method reads data from the current UDP packet into the provided buffer.

#### Parameters
* `buffer` Pointer to the buffer where the data will be stored. 

* `len` The number of bytes to read into the buffer. 

#### Returns
int Returns the number of bytes read, which may be less than len if not enough data is available.
<hr />

### `read` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a3e5de3919a838c551d083d000fa43551" class="anchor"></a>

```cpp
virtual int read(char * buffer, size_t len)
```

Reads a specified number of bytes from the current UDP packet into a character buffer.

This method reads data from the current UDP packet into the provided character buffer.

#### Parameters
* `buffer` Pointer to the character buffer where the data will be stored. 

* `len` The number of bytes to read into the buffer. 

#### Returns
int Returns the number of bytes read, which may be less than len if not enough data is available.
<hr />

### `peek` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a00c33af0714cb9286ca9c022614ecd37" class="anchor"></a>

```cpp
virtual int peek()
```

Peeks at the next byte in the current UDP packet without removing it from the buffer.

This method retrieves the next byte from the current UDP packet buffer without consuming it.

#### Returns
int Returns the next byte, or -1 if no data is available.
<hr />

### `flush` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1aa90a09dd0de4548708561bea2f590db2" class="anchor"></a>

```cpp
virtual void flush()
```

Flushes the current UDP packet buffer.

This method clears the current UDP packet buffer, discarding any unsent data.
<hr />

### `remoteIP` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a1da65ecad341c2867e371992acd4610b" class="anchor"></a>

```cpp
virtual IPAddress remoteIP()
```

Returns the IP address of the remote host that sent the last received packet.

This method retrieves the IP address of the sender of the last received UDP packet.

#### Returns
IPAddress Returns the IP address of the remote host.
<hr />

### `remotePort` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a4137463ae8d926184532274a690733c3" class="anchor"></a>

```cpp
virtual uint16_t remotePort()
```

Returns the port number of the remote host that sent the last received packet.

This method retrieves the port number of the sender of the last received UDP packet.

#### Returns
uint16_t Returns the port number of the remote host.
<hr />

### `onUdpRawRecv` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1a60b49bb4946021917c658ace0d56dd77" class="anchor"></a>

```cpp
void onUdpRawRecv(struct udp_pcb * pcb, struct pbuf * p, const ip_addr_t * addr, uint16_t port)
```

<hr />

### `bufferSize` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1af9fdf26f7aa66527b801f9371788b226" class="anchor"></a>

```cpp
inline void bufferSize(int size)
```

<hr />

# class `MacAddress` <a id="class_mac_address" class="anchor"></a>

```cpp
class MacAddress
  : public arduino::Printable
```

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
| [`MacAddress`](#class_mac_address_1a4a8ad51cb7b7cd3aee80114a899f877f) |  |
| [`MacAddress`](#class_mac_address_1a5d390e3884a7bf521573d81dc7568c5d) |  |
| [`data`](#class_mac_address_1a059cb194dc775450f4db54c459be4dbc) |  |
| [`data`](#class_mac_address_1ac8833ad667c20bb0af07fc02ae5c1b81) |  |
| [`create_from_uid`](#class_mac_address_1a3ea00be39c7c6e07d70bd30cc1a59383) |  |

## Members

### `MacAddress` <a id="class_mac_address_1a4a8ad51cb7b7cd3aee80114a899f877f" class="anchor"></a>

```cpp
inline MacAddress()
```

<hr />

### `MacAddress` <a id="class_mac_address_1a5d390e3884a7bf521573d81dc7568c5d" class="anchor"></a>

```cpp
MacAddress(uint8_t const * mac)
```

<hr />

### `data` <a id="class_mac_address_1a059cb194dc775450f4db54c459be4dbc" class="anchor"></a>

```cpp
inline uint8_t * data()
```

<hr />

### `data` <a id="class_mac_address_1ac8833ad667c20bb0af07fc02ae5c1b81" class="anchor"></a>

```cpp
inline uint8_t const * data() const
```

<hr />

### `create_from_uid` <a id="class_mac_address_1a3ea00be39c7c6e07d70bd30cc1a59383" class="anchor"></a>

```cpp
static MacAddress create_from_uid()
```

<hr />

# class `T1SMacSettings` <a id="class_t1_s_mac_settings" class="anchor"></a>

```cpp
class T1SMacSettings
  : public arduino::Printable
```

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
| [`T1SMacSettings`](#class_t1_s_mac_settings_1a4287ed1602f9fdcffe72cb22c3888ec0) |  |
| [`T1SMacSettings`](#class_t1_s_mac_settings_1a7061757ec4c7ab69b1f4f66dbd0212a4) |  |
| [`printTo`](#class_t1_s_mac_settings_1afd782262ddddd0d38ba89cc7fa148c8d) |  |
| [`isMacPromiscuousModeEnabled`](#class_t1_s_mac_settings_1a72f36124778c9dc82b80dcf262f041c8) |  |
| [`isMacTxCutThroughEnabled`](#class_t1_s_mac_settings_1ab34a1b0e45e3ad08b11c6edf2bad065c) |  |
| [`isMacRxCutThroughEnabled`](#class_t1_s_mac_settings_1ae7fc48fd58968be4de157f9af1a0c5ee) |  |

## Members

### `T1SMacSettings` <a id="class_t1_s_mac_settings_1a4287ed1602f9fdcffe72cb22c3888ec0" class="anchor"></a>

```cpp
inline T1SMacSettings()
```

<hr />

### `T1SMacSettings` <a id="class_t1_s_mac_settings_1a7061757ec4c7ab69b1f4f66dbd0212a4" class="anchor"></a>

```cpp
T1SMacSettings(bool const mac_promiscuous_mode, bool const mac_tx_cut_through, bool const mac_rx_cut_through)
```

<hr />

### `printTo` <a id="class_t1_s_mac_settings_1afd782262ddddd0d38ba89cc7fa148c8d" class="anchor"></a>

```cpp
virtual size_t printTo(Print & p) const
```

<hr />

### `isMacPromiscuousModeEnabled` <a id="class_t1_s_mac_settings_1a72f36124778c9dc82b80dcf262f041c8" class="anchor"></a>

```cpp
inline bool isMacPromiscuousModeEnabled() const
```

<hr />

### `isMacTxCutThroughEnabled` <a id="class_t1_s_mac_settings_1ab34a1b0e45e3ad08b11c6edf2bad065c" class="anchor"></a>

```cpp
inline bool isMacTxCutThroughEnabled() const
```

<hr />

### `isMacRxCutThroughEnabled` <a id="class_t1_s_mac_settings_1ae7fc48fd58968be4de157f9af1a0c5ee" class="anchor"></a>

```cpp
inline bool isMacRxCutThroughEnabled() const
```

<hr />

# class `T1SPlcaSettings` <a id="class_t1_s_plca_settings" class="anchor"></a>

```cpp
class T1SPlcaSettings
  : public arduino::Printable
```

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
| [`T1SPlcaSettings`](#class_t1_s_plca_settings_1a98cd83059eb840b1ad960f688d61b749) |  |
| [`T1SPlcaSettings`](#class_t1_s_plca_settings_1abe2007ae55d1847b646fb2e93a251b5c) |  |
| [`T1SPlcaSettings`](#class_t1_s_plca_settings_1a3639bf3ba8ac9190baf3c7782d6dafe0) |  |
| [`printTo`](#class_t1_s_plca_settings_1a6dd605df1a8952cbed87faec1dbbbba4) |  |
| [`nodeId`](#class_t1_s_plca_settings_1ad5526a2c53ee9c501a664fef31a77c90) |  |
| [`nodeCount`](#class_t1_s_plca_settings_1a2ae01dd9f4191819d2fc00336f5d9f98) |  |
| [`burstCount`](#class_t1_s_plca_settings_1a96ff17ba62f03af113e7b8b682c2cd54) |  |
| [`burstTimer`](#class_t1_s_plca_settings_1a7190546c8dcb115100458e909eeb6894) |  |

## Members

### `T1SPlcaSettings` <a id="class_t1_s_plca_settings_1a98cd83059eb840b1ad960f688d61b749" class="anchor"></a>

```cpp
inline T1SPlcaSettings()
```

<hr />

### `T1SPlcaSettings` <a id="class_t1_s_plca_settings_1abe2007ae55d1847b646fb2e93a251b5c" class="anchor"></a>

```cpp
inline T1SPlcaSettings(uint8_t const node_id)
```

<hr />

### `T1SPlcaSettings` <a id="class_t1_s_plca_settings_1a3639bf3ba8ac9190baf3c7782d6dafe0" class="anchor"></a>

```cpp
T1SPlcaSettings(uint8_t const node_id, uint8_t const node_count, uint8_t const burst_count, uint8_t const burst_timer)
```

<hr />

### `printTo` <a id="class_t1_s_plca_settings_1a6dd605df1a8952cbed87faec1dbbbba4" class="anchor"></a>

```cpp
virtual size_t printTo(Print & p) const
```

<hr />

### `nodeId` <a id="class_t1_s_plca_settings_1ad5526a2c53ee9c501a664fef31a77c90" class="anchor"></a>

```cpp
inline uint8_t nodeId() const
```

<hr />

### `nodeCount` <a id="class_t1_s_plca_settings_1a2ae01dd9f4191819d2fc00336f5d9f98" class="anchor"></a>

```cpp
inline uint8_t nodeCount() const
```

<hr />

### `burstCount` <a id="class_t1_s_plca_settings_1a96ff17ba62f03af113e7b8b682c2cd54" class="anchor"></a>

```cpp
inline uint8_t burstCount() const
```

<hr />

### `burstTimer` <a id="class_t1_s_plca_settings_1a7190546c8dcb115100458e909eeb6894" class="anchor"></a>

```cpp
inline uint8_t burstTimer() const
```

<hr />

# class `Arduino_10BASE_T1S_UDP::UdpRxPacket` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet" class="anchor"></a>

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
| [`UdpRxPacket`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1aacda8f430602f5cb9ff2a7b7280942d9) |  |
| [`remoteIP`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1aa1a20fd0505c95d0937dfb8814bc7c4a) |  |
| [`remotePort`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a70254bf5684f1c304cf9d755a1a2be8e) |  |
| [`totalSize`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a86fb44b5697105edb2e1306a332d2c18) |  |
| [`available`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1af3c026909e38d8c01a9f7494531c883f) |  |
| [`read`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a8d8efe06e6a343b9601f2592fd6ff3f8) |  |
| [`read`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a13ec2494c7c6059f7ec2a2e3f0cff9b7) |  |
| [`read`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1af69ec99a43fcefe2dcac31600e6428fa) |  |
| [`peek`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1adc3afaaf394de25a145549c550602b86) |  |
| [`SharedPtr`](#class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a12fcd44631929c7954872bf2d3fc827d) |  |

## Members

### `UdpRxPacket` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1aacda8f430602f5cb9ff2a7b7280942d9" class="anchor"></a>

```cpp
inline UdpRxPacket(IPAddress const remote_ip, uint16_t const remote_port, uint8_t const * p_data, size_t const data_len)
```

<hr />

### `remoteIP` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1aa1a20fd0505c95d0937dfb8814bc7c4a" class="anchor"></a>

```cpp
inline IPAddress remoteIP() const
```

<hr />

### `remotePort` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a70254bf5684f1c304cf9d755a1a2be8e" class="anchor"></a>

```cpp
inline uint16_t remotePort() const
```

<hr />

### `totalSize` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a86fb44b5697105edb2e1306a332d2c18" class="anchor"></a>

```cpp
inline size_t totalSize() const
```

<hr />

### `available` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1af3c026909e38d8c01a9f7494531c883f" class="anchor"></a>

```cpp
inline int available()
```

<hr />

### `read` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a8d8efe06e6a343b9601f2592fd6ff3f8" class="anchor"></a>

```cpp
inline int read()
```

<hr />

### `read` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a13ec2494c7c6059f7ec2a2e3f0cff9b7" class="anchor"></a>

```cpp
inline int read(unsigned char * buffer, size_t len)
```

<hr />

### `read` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1af69ec99a43fcefe2dcac31600e6428fa" class="anchor"></a>

```cpp
inline int read(char * buffer, size_t len)
```

<hr />

### `peek` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1adc3afaaf394de25a145549c550602b86" class="anchor"></a>

```cpp
inline int peek()
```

<hr />

### `SharedPtr` <a id="class_arduino__10_b_a_s_e___t1_s___u_d_p_1_1_udp_rx_packet_1a12fcd44631929c7954872bf2d3fc827d" class="anchor"></a>

```cpp
typedef SharedPtr
```

<hr />

