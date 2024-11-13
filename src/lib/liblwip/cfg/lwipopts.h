/**
 * @file
 *
 * lwIP Options Configuration
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#ifndef NO_SYS
#define NO_SYS                          1
#endif

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#ifndef SYS_LIGHTWEIGHT_PROT
#define SYS_LIGHTWEIGHT_PROT            0
#endif

/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/
/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT                   4
#endif


/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
#ifndef MEM_SIZE
#define MEM_SIZE                        0
#endif


#ifndef MEM_LIBC_MALLOC
#define MEM_LIBC_MALLOC                  1
#endif

#ifndef LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT
#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT   0
#endif

/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/
/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#ifndef MEMP_NUM_PBUF
#define MEMP_NUM_PBUF                   32
#endif

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#ifndef MEMP_NUM_RAW_PCB
#define MEMP_NUM_RAW_PCB                4
#endif

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#ifndef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB                4
#endif

/**
 * MEMP_NUM_TCP_PCB: the number of simultaneously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB                0
#endif

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN         0
#endif

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG                0
#endif

/**
 * MEMP_NUM_ARP_QUEUE: the number of simultaneouslysimultaneously queued outgoing
 * packets (pbufs) that are waiting for an ARP request (to resolve
 * their destination address) to finish.
 * (requires the ARP_QUEUEING option)
 */
#ifndef MEMP_NUM_ARP_QUEUE
#define MEMP_NUM_ARP_QUEUE              6
#endif

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active timeouts.
 * The default number of timeouts is calculated here for all enabled modules.
 * The formula expects settings to be either '0' or '1'.
 *
 * To this default value, 1 was added for the snmp_increment timer.
 */
#ifndef MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_SUPPORT + (LWIP_IPV6 ? (1 + LWIP_IPV6_REASS + LWIP_IPV6_MLD) : 0)) + 1
#endif

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifndef MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_NETBUF                 0
#endif

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifndef MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN                0
#endif

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication.
 * (only needed if you use tcpip.c)
 */
#ifndef MEMP_NUM_TCPIP_MSG_API
#define MEMP_NUM_TCPIP_MSG_API          0
#endif

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 */
#ifndef MEMP_NUM_TCPIP_MSG_INPKT
#define MEMP_NUM_TCPIP_MSG_INPKT        0
#endif

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
 */
#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE                  4
#endif

/*
   ---------------------------------
   ---------- ARP options ----------
   ---------------------------------
*/
/**
 * LWIP_ARP==1: Enable ARP functionality.
 */
#ifndef LWIP_ARP
#define LWIP_ARP                        1
#endif

/*
   --------------------------------
   ---------- IP options ----------
   --------------------------------
*/
#ifndef LWIP_IPV4
#define LWIP_IPV4                       1
#endif
#ifndef LWIP_IPV6
#define LWIP_IPV6                       0
#endif

/**
 * IP_FORWARD==1: Enables the ability to forward IP packets across network
 * interfaces. If you are going to run lwIP on a device with only one network
 * interface, define this to 0.
 */
#ifndef IP_FORWARD
#define IP_FORWARD                      0
#endif

/**
 * IP_OPTIONS: Defines the behavior for IP options.
 *      IP_OPTIONS==0_ALLOWED: All packets with IP options are dropped.
 *      IP_OPTIONS==1_ALLOWED: IP options are allowed (but not parsed).
 */
#ifndef IP_OPTIONS_ALLOWED
#define IP_OPTIONS_ALLOWED              1
#endif

/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#ifndef IP_REASSEMBLY
#define IP_REASSEMBLY                   0
#endif

/**
 * IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#ifndef IP_FRAG
#define IP_FRAG                         0
#endif

/**
 * IP_REASS_MAXAGE: Maximum time (in multiples of IP_TMR_INTERVAL - so seconds, normally)
 * a fragmented IP packet waits for all fragments to arrive. If not all fragments arrived
 * in this time, the whole packet is discarded.
 */
#ifndef IP_REASS_MAXAGE
#define IP_REASS_MAXAGE                 3
#endif

/**
 * IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled.
 * Since the received pbufs are enqueued, be sure to configure
 * PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive
 * packets even if the maximum amount of fragments is enqueued for reassembly!
 */
#ifndef IP_REASS_MAX_PBUFS
#define IP_REASS_MAX_PBUFS              1
#endif

/**
 * IP_FRAG_USES_STATIC_BUF==1: Use a static MTU-sized buffer for IP
 * fragmentation. Otherwise pbufs are allocated and reference the original
 * packet data to be fragmented.
 */
#ifndef IP_FRAG_USES_STATIC_BUF
#define IP_FRAG_USES_STATIC_BUF         0
#endif

/**
 * IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.
 */
#ifndef IP_DEFAULT_TTL
#define IP_DEFAULT_TTL                  255
#endif

/*
   ----------------------------------
   ---------- ICMP options ----------
   ----------------------------------
*/
/**
 * LWIP_ICMP==1: Enable ICMP module inside the IP stack.
 * Be careful, disable that make your product non-compliant to RFC1122
 */
#ifndef LWIP_ICMP
#define LWIP_ICMP                       1
#endif

/**
 * ICMP_TTL: Default value for Time-To-Live used by ICMP packets.
 */
#ifndef ICMP_TTL
#define ICMP_TTL                       (IP_DEFAULT_TTL)
#endif

/*
   ---------------------------------
   ---------- RAW options ----------
   ---------------------------------
*/
/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#ifndef LWIP_RAW
#define LWIP_RAW                        1
#endif

/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
/**
 * LWIP_DHCP==1: Enable DHCP module.
 */
#ifndef LWIP_DHCP
#define LWIP_DHCP                       0
#endif

/*
   ------------------------------------
   ---------- AUTOIP options ----------
   ------------------------------------
*/
/**
 * LWIP_AUTOIP==1: Enable AUTOIP module.
 */
#ifndef LWIP_AUTOIP
#define LWIP_AUTOIP                     0
#endif

/*
   ----------------------------------
   ---------- SNMP options ----------
   ----------------------------------
*/
/**
 * LWIP_SNMP==1: Turn on SNMP module. UDP must be available for SNMP
 * transport.
 */
#ifndef LWIP_SNMP
#define LWIP_SNMP                       0
#endif
#ifndef LWIP_MIB2_CALLBACKS
#define LWIP_MIB2_CALLBACKS             0
#endif
#ifndef MIB2_STATS
#define MIB2_STATS                      0
#endif

/*
   ----------------------------------
   ---------- IGMP options ----------
   ----------------------------------
*/
/**
 * LWIP_IGMP==1: Turn on IGMP module.
 */
#ifndef LWIP_IGMP
#define LWIP_IGMP                       0
#endif

/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#ifndef LWIP_DNS
#define LWIP_DNS                        0
#endif

/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/
/**
 * LWIP_UDP==1: Turn on UDP.
 */
#ifndef LWIP_UDP
#define LWIP_UDP                        1
#endif

/**
 * LWIP_UDPLITE==1: Turn on UDP-Lite. (Requires LWIP_UDP)
 */
#ifndef LWIP_UDPLITE
#define LWIP_UDPLITE                    0
#endif

/**
 * UDP_TTL: Default Time-To-Live value.
 */
#ifndef UDP_TTL
#define UDP_TTL                         (IP_DEFAULT_TTL)
#endif

/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
/**
 * LWIP_TCP==1: Turn on TCP.
 */
#ifndef LWIP_TCP
#define LWIP_TCP                        0
#endif

/*
   ----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------
*/
/**
 * PBUF_LINK_HLEN: the number of bytes that should be allocated for a
 * link level header. The default is 14, the standard value for
 * Ethernet.
 */
#ifndef PBUF_LINK_HLEN
#define PBUF_LINK_HLEN                  14
#endif

/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
*/
/**
 * LWIP_HAVE_LOOPIF==1: Support loop interface (127.0.0.1) and loopif.c
 */
#ifndef LWIP_HAVE_LOOPIF
#define LWIP_HAVE_LOOPIF                0
#endif


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/

/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#ifndef LWIP_NETCONN
#define LWIP_NETCONN                    0
#endif

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#ifndef LWIP_SOCKET
#define LWIP_SOCKET                     0
#endif

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */

#ifndef LWIP_STATS_DISPLAY
#define LWIP_STATS_DISPLAY              0
#endif
#ifndef LWIP_STATS
#define LWIP_STATS                      0
#endif
#ifndef ETHARP_STATS
#define ETHARP_STATS                    0
#endif
#ifndef IP_STATS
#define IP_STATS                        0
#endif
#ifndef UDP_STATS
#define UDP_STATS                       0
#endif
#ifndef TCP_STATS
#define TCP_STATS                       0
#endif


#ifndef LWIP_NETIF_STATUS_CALLBACK
#define LWIP_NETIF_STATUS_CALLBACK      1
#endif



#ifndef CHECKSUM_CHECK_IP
#define CHECKSUM_CHECK_IP               0
#endif
#ifndef CHECKSUM_CHECK_UDP
#define CHECKSUM_CHECK_UDP              0
#endif
#ifndef CHECKSUM_CHECK_TCP
#define CHECKSUM_CHECK_TCP              0
#endif

/*
 * Hardware Options
 */

/*
 * MTU: Maximum Transfer Unit 1500 for a standart Ethernet connection
 */
#ifndef ETHERNET_MTU
#define ETHERNET_MTU                    1500
#endif

/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/

//#define LWIP_DEBUG                      0

#define TAPIF_DEBUG      LWIP_DBG_OFF
#define TUNIF_DEBUG      LWIP_DBG_OFF
#define UNIXIF_DEBUG     LWIP_DBG_OFF
#define DELIF_DEBUG      LWIP_DBG_OFF
#define SIO_FIFO_DEBUG   LWIP_DBG_OFF
#define TCPDUMP_DEBUG    LWIP_DBG_OFF
#define API_LIB_DEBUG    LWIP_DBG_OFF
#define API_MSG_DEBUG    LWIP_DBG_OFF
#define TCPIP_DEBUG      LWIP_DBG_OFF
#define NETIF_DEBUG      LWIP_DBG_OFF
#define SOCKETS_DEBUG    LWIP_DBG_OFF
#define DEMO_DEBUG       LWIP_DBG_OFF
#define IP_DEBUG         LWIP_DBG_OFF
#define IP_REASS_DEBUG   LWIP_DBG_OFF
#define RAW_DEBUG        LWIP_DBG_OFF
#define ICMP_DEBUG       LWIP_DBG_OFF
#define UDP_DEBUG        LWIP_DBG_OFF
#define TCP_DEBUG        LWIP_DBG_OFF
#define TCP_INPUT_DEBUG  LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG LWIP_DBG_OFF
#define TCP_RTO_DEBUG    LWIP_DBG_OFF
#define TCP_CWND_DEBUG   LWIP_DBG_OFF
#define TCP_WND_DEBUG    LWIP_DBG_OFF
#define TCP_FR_DEBUG     LWIP_DBG_OFF
#define TCP_QLEN_DEBUG   LWIP_DBG_OFF
#define TCP_RST_DEBUG    LWIP_DBG_OFF
#define ETHARP_DEBUG     LWIP_DBG_OFF
#define DHCP_DEBUG       LWIP_DBG_OFF
#define IP6_DEBUG        LWIP_DBG_OFF

extern unsigned char debug_flags;
#define LWIP_DBG_TYPES_ON debug_flags

// Rename APIs / structs to avoid clashes
#define sys_now t1s_sys_now
#define etharp_cleanup_netif t1s_etharp_cleanup_netif
#define etharp_find_addr t1s_etharp_find_addr
#define etharp_get_entry t1s_etharp_get_entry
#define etharp_input t1s_etharp_input
#define etharp_request t1s_etharp_request
#define etharp_tmr t1s_etharp_tmr
#define etharp_query t1s_etharp_query
#define etharp_output t1s_etharp_output
#define icmp_input t1s_icmp_input
#define icmp_dest_unreach t1s_icmp_dest_unreach
#define ip4_route t1s_ip4_route
#define ip4_input t1s_ip4_input
#define ip4_output_if t1s_ip4_output_if
#define ip4_output_if_src t1s_ip4_output_if_src
#define ip4_output t1s_ip4_output
#define ip4_addr_isbroadcast_u32 t1s_ip4_addr_isbroadcast_u32
#define ip4_addr_netmask_valid t1s_ip4_addr_netmask_valid
#define ip4addr_aton t1s_ip4addr_aton
#define ipaddr_addr t1s_ipaddr_addr
#define ip4addr_ntoa_r t1s_ip4addr_ntoa_r
#define ip4addr_ntoa t1s_ip4addr_ntoa
#define ip_addr_broadcast t1s_ip_addr_broadcast
#define ip_addr_any t1s_ip_addr_any
#define lwip_strnstr t1s_lwip_strnstr
#define lwip_stricmp t1s_lwip_stricmp
#define lwip_strnicmp t1s_lwip_strnicmp
#define lwip_itoa t1s_lwip_itoa
#define inet_chksum_pseudo t1s_inet_chksum_pseudo
#define ip_chksum_pseudo t1s_ip_chksum_pseudo
#define inet_chksum_pseudo_partial t1s_inet_chksum_pseudo_partial
#define ip_chksum_pseudo_partial t1s_ip_chksum_pseudo_partial
#define inet_chksum t1s_inet_chksum
#define inet_chksum_pbuf t1s_inet_chksum_pbuf
#define lwip_init t1s_lwip_init
#define mem_init t1s_mem_init
#define mem_free t1s_mem_free
#define mem_trim t1s_mem_trim
#define mem_malloc t1s_mem_malloc
#define mem_calloc t1s_mem_calloc
#define memp_init_pool t1s_memp_init_pool
#define memp_init t1s_memp_init
#define memp_malloc_pool t1s_memp_malloc_pool
#define memp_malloc t1s_memp_malloc
#define memp_free_pool t1s_memp_free_pool
#define memp_free t1s_memp_free
#define memp_pools t1s_memp_pools
#define memp_PBUF_POOL t1s_memp_PBUF_POOL
#define memp_PBUF t1s_memp_PBUF
#define memp_SYS_TIMEOUT t1s_memp_SYS_TIMEOUT
#define memp_UDP_PCB t1s_memp_UDP_PCB
#define netif_init t1s_netif_init
#define netif_input t1s_netif_input
#define netif_set_ipaddr t1s_netif_set_ipaddr
#define netif_set_netmask t1s_netif_set_netmask
#define netif_set_gw t1s_netif_set_gw
#define netif_set_addr t1s_netif_set_addr
#define netif_add t1s_netif_add
#define netif_add_noaddr t1s_netif_add_noaddr
#define netif_set_default t1s_netif_set_default
#define netif_set_up t1s_netif_set_up
#define netif_set_down t1s_netif_set_down
#define netif_remove t1s_netif_remove
#define netif_set_status_callback t1s_netif_set_status_callback
#define netif_set_link_up t1s_netif_set_link_up
#define netif_set_link_down t1s_netif_set_link_down
#define netif_get_by_index t1s_netif_get_by_index
#define netif_index_to_name t1s_netif_index_to_name
#define netif_find t1s_netif_find
#define netif_name_to_index t1s_netif_name_to_index
#define pbuf_alloc_reference t1s_pbuf_alloc_reference
#define pbuf_add_header t1s_pbuf_add_header
#define pbuf_add_header_force t1s_pbuf_add_header_force
#define pbuf_remove_header t1s_pbuf_remove_header
#define pbuf_header t1s_pbuf_header
#define pbuf_header_force t1s_pbuf_header_force
#define pbuf_free_header t1s_pbuf_free_header
#define pbuf_clen t1s_pbuf_clen
#define pbuf_cat t1s_pbuf_cat
#define pbuf_chain t1s_pbuf_chain
#define pbuf_dechain t1s_pbuf_dechain
#define pbuf_copy t1s_pbuf_copy
#define pbuf_copy_partial t1s_pbuf_copy_partial
#define pbuf_get_contiguous t1s_pbuf_get_contiguous
#define pbuf_skip t1s_pbuf_skip
#define pbuf_take t1s_pbuf_take
#define pbuf_take_at t1s_pbuf_take_at
#define pbuf_clone t1s_pbuf_clone
#define pbuf_coalesce t1s_pbuf_coalesce
#define pbuf_try_get_at t1s_pbuf_try_get_at
#define pbuf_get_at t1s_pbuf_get_at
#define pbuf_put_at t1s_pbuf_put_at
#define pbuf_memcmp t1s_pbuf_memcmp
#define pbuf_memfind t1s_pbuf_memfind
#define pbuf_strstr t1s_pbuf_strstr
#define sys_timeout t1s_sys_timeout
#define sys_timeouts_init t1s_sys_timeouts_init
#define sys_untimeout t1s_sys_untimeout
#define sys_check_timeouts t1s_sys_check_timeouts
#define sys_restart_timeouts t1s_sys_restart_timeouts
#define sys_timeouts_sleeptime t1s_sys_timeouts_sleeptime
#define lwip_num_cyclic_timers t1s_lwip_num_cyclic_timers
#define lwip_cyclic_timers t1s_lwip_cyclic_timers
#define udp_init t1s_udp_init
#define udp_input t1s_udp_input
#define udp_bind t1s_udp_bind
#define udp_sendto_if_src t1s_udp_sendto_if_src
#define udp_sendto_if t1s_udp_sendto_if
#define udp_sendto t1s_udp_sendto
#define udp_send t1s_udp_send
#define udp_bind_netif t1s_udp_bind_netif
#define udp_connect t1s_udp_connect
#define udp_disconnect t1s_udp_disconnect
#define udp_recv t1s_udp_recv
#define udp_remove t1s_udp_remove
#define udp_new t1s_udp_new
#define udp_new_ip_type t1s_udp_new_ip_type
#define udp_netif_ip_addr_changed t1s_udp_netif_ip_addr_changed
#define bridgeif_fdb_update_src t1s_bridgeif_fdb_update_src
#define bridgeif_fdb_get_dst_ports t1s_bridgeif_fdb_get_dst_ports
#define bridgeif_fdb_init t1s_bridgeif_fdb_init
#define ethernet_input t1s_ethernet_input
#define ethernet_output t1s_ethernet_output
#define ethzero t1s_ethzero
#define ethbroadcast t1s_ethbroadcast
#define pbuf_realloc t1s_pbuf_realloc
#define pbuf_free t1s_pbuf_free
#define pbuf_alloc t1s_pbuf_alloc
#define pbuf_ref t1s_pbuf_ref
#define etharp_cleanup_netif t1s_etharp_cleanup_netif
#define etharp_find_addr t1s_etharp_find_addr
#define etharp_get_entry t1s_etharp_get_entry
#define etharp_input t1s_etharp_input
#define etharp_request t1s_etharp_request
#define etharp_tmr t1s_etharp_tmr
#define etharp_query t1s_etharp_query
#define etharp_output t1s_etharp_output
#define icmp_input t1s_icmp_input
#define icmp_dest_unreach t1s_icmp_dest_unreach

#endif /* LWIP_LWIPOPTS_H */