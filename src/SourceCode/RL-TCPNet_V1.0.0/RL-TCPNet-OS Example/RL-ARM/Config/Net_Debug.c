/*----------------------------------------------------------------------------
 *      RL-ARM - TCPnet
 *----------------------------------------------------------------------------
 *      Name:    NET_DEBUG.C
 *      Purpose: Debug Module
 *      Rev.:    V4.71
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <Net_Config.h>

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

// <q>Print Time Stamp
// ===================
//   <i> Enable printing the time-info in debug messages
#define DBG_TIME       1

// <h>TCPnet Debug Definitions
// ===========================
//   <o>Memory Management Debug <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off Dynamic Memory debug messages
#define DBG_MEM        0

//   <o>Ethernet Debug <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off Ethernet debug messages
#define DBG_ETH        0

//   <o>PPP Debug   <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off PPP debug messages
#define DBG_PPP        0

//   <o>SLIP Debug  <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off SLIP debug messages
#define DBG_SLIP       0

//   <o>ARP Debug   <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off ARP debug messages
#define DBG_ARP        0

//   <o>IP Debug    <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off IP debug messages
#define DBG_IP         0

//   <o>ICMP Debug  <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off ICMP debug messages
#define DBG_ICMP       0

//   <o>IGMP Debug  <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off IGMP debug messages
#define DBG_IGMP       0

//   <o>UDP Debug   <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off UDP debug messages
#define DBG_UDP        0

//   <o>TCP Debug   <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off TCP debug messages
#define DBG_TCP        0

//   <o>NBNS Debug  <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off NetBIOS Name Service debug messages
#define DBG_NBNS       0

//   <o>DHCP Debug  <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off Dynamic Host Configuration debug messages
#define DBG_DHCP       0

//   <o>DNS Debug   <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off Domain Name Service debug messages
#define DBG_DNS        0

//   <o>SNMP Debug  <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off Simple Network Management debug messages
#define DBG_SNMP       0

//   <o>SNTP Debug  <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off Simple Network Time debug messages
#define DBG_SNTP       0

//   <o>BSD Debug   <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off BSD Interface debug messages
#define DBG_BSD        0

// </h>
// <h>Application Debug Definitions
// ================================
//   <o>HTTP Server Debug <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off Web Server debug messages
#define DBG_HTTP       0

//   <o>FTP Server Debug <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off FTP Server debug messages
#define DBG_FTP        0

//   <o>FTP Client Debug <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off FTP Client debug messages
#define DBG_FTPC       0

//   <o>Telnet Server Debug <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off Telnet Server debug messages
#define DBG_TNET       0

//   <o>TFTP Server Debug <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off TFTP Server debug messages
#define DBG_TFTP       0

//   <o>TFTP Client Debug <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off TFTP Client debug messages
#define DBG_TFTPC      0

//   <o>SMTP Client Debug <0=> Off <1=> Errors only <2=> Full debug
//   <i> Turn On/Off SMTP Client debug messages
#define DBG_SMTP       0

// </h>

//------------- <<< end of configuration section >>> -----------------------


/*--------------------------- init_debug ------------------------------------*/

void init_debug (void) {
  /* Add your code to initialize the Debug output. This is usually the  */
  /* serial interface. The function is called at TCPnet system startup. */
  /* You may need to customize also the 'putchar()' function.           */

}


/*----------------------------------------------------------------------------
 *      TCPnet Debug Functions
 *---------------------------------------------------------------------------*/

#define  __NET_DEBUG__

#include <Net_lib.c>

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
