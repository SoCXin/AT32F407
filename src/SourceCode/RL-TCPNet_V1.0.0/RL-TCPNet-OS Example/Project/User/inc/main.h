/**
 ******************************************************************************
 * @file        ETH/iperf/main.h
 * @brief       This file contains all the functions prototypes for the 
                ethernet function
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
#ifndef __MAIN_H
#define __MAIN_H

#include "stdio.h"
#include "at32f4xx_eth.h"
#include "at32f4xx_rtc.h"
#include "at32f4xx_pwr.h"

#define SYSTEMTICK_PERIOD_MS  10

void System_Periodic_Handle(void);
#endif
