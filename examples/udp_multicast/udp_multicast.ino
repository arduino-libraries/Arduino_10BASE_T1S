/**
 * @file 
 * @author Chace0219 (chace0219@gmail.com)
 * 
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_10BASE_T1S.h>

#include <SPI.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static uint8_t const T1S_PLCA_NODE_ID = 1;

// static IPAddress const ip_addr     {192, 168,  137, 100};
// static IPAddress const network_mask{255, 255, 255,   0};
// static IPAddress const gateway     {192, 168,  137, 1};

/* All zeroes — DHCP will supply the real address after PLCA comes up.
 * Windows ICS assigns from 192.168.137.x by default. */
static IPAddress const ip_addr     {0, 0, 0, 0};
static IPAddress const network_mask{0, 0, 0, 0};
static IPAddress const gateway     {0, 0, 0, 0};

static T1SPlcaSettings const t1s_plca_settings{T1S_PLCA_NODE_ID};
static T1SMacSettings const t1s_default_mac_settings;

static IPAddress const UDP_MULTICAST_ADDR = {239, 1, 2, 3}; /* 10BASE-T1S local multicast group */
static uint16_t const UDP_PORT = 8888;

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

const int8_t CS_LAN8651_PIN = 17;
const int8_t RST_LAN8651_PIN = 21;
const int8_t IRQ_LAN8651_PIN = 20;
Arduino_10BASE_T1S_PHY_TC6(SPI, CS_LAN8651_PIN, RST_LAN8651_PIN, IRQ_LAN8651_PIN);

Arduino_10BASE_T1S_UDP udp_client;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/
static void OnPlcaStatus(bool success, bool plcaStatus);
void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);

  /* Initialize digital IO interface for interfacing
   * with the LAN8651.
   */
  pinMode(IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN),
                  []() { t1s_io.onInterrupt(); },
                  FALLING);

  /* Initialize IO module. */
  if (!t1s_io.begin())
  {
    Serial.println("'TC6_Io::begin(...)' failed.");
    for (;;) { }
  }

  MacAddress const mac_addr = MacAddress::create_from_uid();

  if (!t1s_phy.begin(ip_addr
    , network_mask
    , gateway
    , mac_addr
    , t1s_plca_settings
    , t1s_default_mac_settings))
  {
    Serial.println("'TC6::begin(...)' failed.");
    for (;;) { }
  }

  Serial.print("MAC\t");
  Serial.println(mac_addr);
  Serial.println(t1s_plca_settings);
  Serial.println(t1s_default_mac_settings);

  /* If DHCP is active (IP was 0.0.0.0), wait until a lease is obtained.
   * PLCA must be active before any Ethernet frames can flow, so we run
   * the same PLCA status / re-enable logic here that the main loop uses. */
  if (ip_addr == IPAddress(0, 0, 0, 0))
  {
    Serial.println("Waiting for DHCP lease (PLCA must come up first)...");
    unsigned long last_plca_ms  = 0;
    unsigned long last_dot_ms   = 0;

    while (t1s_phy.localIP() == IPAddress(0, 0, 0, 0))
    {
      /* Service the TC6 hardware and lwIP stack as fast as possible. */
      t1s_phy.service();

      auto const now = millis();

      /* Check PLCA status every second and re-enable if in CSMA/CD fallback. */
      if ((now - last_plca_ms) >= 1000UL)
      {
        last_plca_ms = now;
        if (!t1s_phy.getPlcaStatus(OnPlcaStatus))
          Serial.println("[DHCP wait] getPlcaStatus() failed");
      }

      /* Print a progress dot every 2 s so the serial isn't flooded. */
      if ((now - last_dot_ms) >= 2000UL)
      {
        last_dot_ms = now;
        Serial.print(".");
      }
    }
    Serial.println();
    Serial.print("DHCP IP\t");
    Serial.println(t1s_phy.localIP());
  }
  else
  {
    Serial.print("Static IP\t");
    Serial.println(ip_addr);
  }

  /* Start receiving on the multicast group.
   * beginMulticast() binds the port AND joins the IGMP group in one call.
   * Must be called AFTER DHCP has assigned an IP — IGMP needs a real source address. */
  // if (!udp_client.begin(UDP_PORT))
  if (!udp_client.beginMulticast(UDP_MULTICAST_ADDR, UDP_PORT))
  {
    Serial.println("beginMulticast(...) failed for UDP client");
    for (;;) { }
  }

  Serial.print("UDP multicast group: ");
  Serial.println(UDP_MULTICAST_ADDR);
  Serial.println("UDP_Client");
}

void loop()
{
  /* Services the hardware and the protocol stack.
   * Must be called cyclic. The faster the better.
   */
  t1s_phy.service();

  static unsigned long prev_beacon_check = 0;
  static unsigned long prev_udp_packet_sent = 0;

  auto const now = millis();

  if ((now - prev_beacon_check) > 1000)
  {
    prev_beacon_check = now;
    if (!t1s_phy.getPlcaStatus(OnPlcaStatus))
      Serial.println("getPlcaStatus(...) failed");
  }

  if ((now - prev_udp_packet_sent) > 2000)
  {
    static int tx_packet_cnt = 0;

    prev_udp_packet_sent = now;

    /* Prepare UDP packet. */
    uint8_t udp_tx_msg_buf[256] = {0};
    int const tx_packet_size = snprintf((char *)udp_tx_msg_buf, sizeof(udp_tx_msg_buf), "Single-Pair Ethernet / 10BASE-T1S: packet cnt = %d", tx_packet_cnt);

    /* Send the UDP packet to the multicast group. Every node on the
     * 10BASE-T1S segment that has joined 239.1.2.3 will receive it. */
    udp_client.beginPacket(UDP_MULTICAST_ADDR, UDP_PORT);
    udp_client.write(udp_tx_msg_buf, tx_packet_size);
    udp_client.endPacket();

    Serial.print("[");
    Serial.print(millis());
    Serial.print("] UDP_Client sending: \"");
    Serial.print(reinterpret_cast<char *>(udp_tx_msg_buf));
    Serial.println("\"");

    tx_packet_cnt++;
  }

  /* Check for incoming UDP packets. */
  int const rx_packet_size = udp_client.parsePacket();
  if (rx_packet_size)
  {
    /* Print some metadata from received UDP packet. */
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] Received ");
    Serial.print(rx_packet_size);
    Serial.print(" bytes from ");
    Serial.print(udp_client.remoteIP());
    Serial.print(" port ");
    Serial.print(udp_client.remotePort());
    Serial.print(", data = \"");

    /* Read from received UDP packet. */
    size_t const UDP_RX_MSG_BUF_SIZE = 16 + 1; /* Reserve the last byte for the '\0' termination. */
    uint8_t udp_rx_msg_buf[UDP_RX_MSG_BUF_SIZE] = {0};
    int bytes_read = udp_client.read(udp_rx_msg_buf, UDP_RX_MSG_BUF_SIZE - 1);
    while(bytes_read != 0)
    {
      /* Print received data to Serial. */
      udp_rx_msg_buf[bytes_read] = '\0'; /* Terminate buffer so that we can print it as a C-string. */
      Serial.print(reinterpret_cast<char *>(udp_rx_msg_buf));

      /* Continue reading. */
      bytes_read = udp_client.read(udp_rx_msg_buf, UDP_RX_MSG_BUF_SIZE - 1);
    }
    Serial.println("\"");

    /* Finish reading the current packet. */
    udp_client.flush();
  }
}

static void OnPlcaStatus(bool success, bool plcaStatus)
{
  if (!success)
  {
    Serial.println("PLCA status register read failed");
    return;
  }

  if (plcaStatus)
  {
    /* Restart DHCP the first time PLCA is confirmed active so that a
     * fresh Discover goes out on a live link instead of relying on
     * lwIP's exponential back-off timer from before the link was up. */
    static bool dhcp_restarted = false;
    if (!dhcp_restarted)
    {
      dhcp_restarted = true;
      t1s_phy.restartDhcp();
      Serial.println("[DHCP] restarted on live PLCA link");
    }
  }
  else {
    Serial.println("CSMA/CD fallback");
    t1s_phy.enablePlca();
  }
}
