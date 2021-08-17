/**
 ******************************************************************************
 * @file        ETH/iperf/eth_config.h
 * @brief       This file contains all the functions prototypes for the ethernet
                configuration related.
 * @author      Artery Technology
 * @date        2019-01-04
 * @version     V1.1.2
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, ARTERYTEK SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2019 ArteryTek</center></h2>
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EHT_CONFIG_H
#define __ETH_CONFIG_H

#include "RTL.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* ETH Memory Buffer configuration. */
#define NUM_RX_BUF          4           /* 0x1800 for Rx (4*1536=6K)         */
#define NUM_TX_BUF          2           /* 0x0600 for Tx (2*1536=3K)         */
#define ETH_BUF_SIZE        1536        /* ETH Receive/Transmit buffer size  */

/* DMA Descriptors. */
typedef struct {
  U32 volatile Stat;
  U32 Ctrl;
  U32 Addr;
  U32 Next;
} RX_Desc;

typedef struct {
  U32 volatile CtrlStat;
  U32 Size;
  U32 Addr;
  U32 Next;
} TX_Desc;   
   
void System_Setup(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void init_ethernet(void);
void interrupt_ethernet (void);
void int_enable_eth (void);
void int_disable_eth (void);
#endif
