/**
 ******************************************************************************
 * @file        ETH/telnet/main.c
 * @brief       ethernet function test.
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

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "stdbool.h"
#include "at32f4xx.h"
#include "at32_board.h"
#include "main.h"
#include "eth_config.h"
#include "Net_Config.h"
  
BOOL tick;
U64 tcp_stack[3200/8];

void LED_Init(void)
{
	GPIO_InitType GPIO_InitStructure;
  /*Enable the LED Clock*/
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD, ENABLE);
  /*Configure the LED pin as ouput push-pull*/
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13;				 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;	
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void LED_Off(void)
{
	GPIO_SetBits(GPIOD,GPIO_Pins_13); 
}

void LED_On(void)
{
	GPIO_ResetBits(GPIOD,GPIO_Pins_13); 
}

__task void user_task (void) 
{
	LED_Init();
	while(1)
	{
		LED_On();
    os_dly_wait(50);
		LED_Off();
    os_dly_wait(50);
	}
}

__task void timer_task (void) {
  /* System tick timer task */
  os_itv_set (10);
  while (1) {
    timer_tick ();
    tick = __TRUE;
    os_itv_wait ();
  }
}

__task void tcp_task (void) {
  /* Main Thread of the TcpNet. This task should have */
	os_dly_wait(50);
	init_TcpNet ();
  while (1) 
	{	
//		timer_tick ();
    main_TcpNet();
		os_tsk_pass();
  }
}

__task void init (void) {
	os_tsk_prio_self (100);
	os_tsk_create (user_task, 2);
	os_tsk_create (timer_task, 31);
	os_tsk_create_user (tcp_task, 1, &tcp_stack, sizeof(tcp_stack));
	os_tsk_delete_self();
	while(1)
	{
		os_dly_wait(100);
	}
}



int main(void)
{
  /* --------BSP Init ------------------------------------------------*/
//  AT32_Board_Init();
//  UART_Print_Init(115200);
  System_Setup();
	os_sys_init(init);
//  init_TcpNet();
  while(1) 
  {
//    timer_tick();
//    main_TcpNet();
  }
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  printf("Wrong parameters value: file %s on line %d\r\n", file, line);

  /* Infinite loop */
  while (1)
  {}
}
#endif


void sys_error (ERROR_CODE code) {
  /* This function is called when a fatal error is encountered. The normal */
  /* program execution is not possible anymore. Add your crytical error   .*/
  /* handler code here.                                                    */

  switch (code) {
    case ERR_MEM_ALLOC:
      /* Out of memory. */
      break;

    case ERR_MEM_FREE:
      /* Trying to release non existing memory block. */
      break;

    case ERR_MEM_CORRUPT:
      /* Memory Link pointer is Corrupted. */
      /* More data written than the size of allocated mem block. */
      break;

    case ERR_MEM_LOCK:
      /* Locked Memory management function (alloc/free) re-entered. */
      /* RTX multithread protection malfunctioning, not implemented */
      /* or interrupt disable is not functioning correctly. */
      break;

    case ERR_UDP_ALLOC:
      /* Out of UDP Sockets. */
      break;

    case ERR_TCP_ALLOC:
      /* Out of TCP Sockets. */
      break;

    case ERR_TCP_STATE:
      /* TCP State machine in undefined state. */
      break;
  }

  /* End-less loop */
  while (1);
}

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2019 ArteryTek *****END OF FILE****/

