/**
 ******************************************************************************
 * @file        ETH/iperf/eth_config.c
 * @brief       Ethernet configuration related.
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
#include "at32f4xx_rcc.h"
#include "at32f4xx_eth.h" 
#include "eth_config.h"
#include "at32_board.h"
#include "Net_Config.h"

/* Macro ----------------------------------------------------------*/
#define DMA_RX_ERROR_MASK   (ETH_DMARxDesc_ES | ETH_DMARxDesc_LE | ETH_DMARxDesc_RWT | \
                             ETH_DMARxDesc_RE | ETH_DMARxDesc_CE)
                             
#define DMA_RX_SEG_MASK     (ETH_DMARxDesc_FS | ETH_DMARxDesc_LS)

#define PHY_ADDRESS       0x00 /* Relative to AT32F407-EVAL Board */

//#define MII_MODE          /* MII mode for AT32F407-EVAL Board (MB784) (check jumpers setting) */
#define RMII_MODE       /* RMII mode for AT32F407-EVAL Board (MB784) (check jumpers setting) */
//#define CRYSTAL_ON_PHY  
#define MII_RX_REMAP 1
//#define ADC_NECESSARY
#define ETH_TOUT		50

/* Gloable variables ---------------------------------------------------------*/
extern U8 own_hw_adr[];

static U8 TxBufIndex;
static U8 RxBufIndex;

static RX_Desc Rx_Desc[NUM_RX_BUF];
static TX_Desc Tx_Desc[NUM_TX_BUF];

static U32 rx_buf[NUM_RX_BUF][ETH_BUF_SIZE>>2];
static U32 tx_buf[NUM_TX_BUF][ETH_BUF_SIZE>>2];

/* Gloable variables ---------------------------------------------------------*/
static void rx_descr_init (void);
static void tx_descr_init (void);

void static Reset_Phy(void);
/**
  * @brief  Setup AT32 system (clocks, Ethernet, GPIO, NVIC) and resources.
  * @param  None
  * @retval None
  */
void System_Setup(void)
{
//  RCC_ClockType RCC_Clocks;

  /* Enable ETHERNET clock  */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_ETHMAC | RCC_AHBPERIPH_ETHMACTX |
                        RCC_AHBPERIPH_ETHMACRX, ENABLE);

  /* Enable GPIOs and ADC1 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_GPIOC |
                         RCC_APB2PERIPH_GPIOD | RCC_APB2PERIPH_GPIOE | RCC_APB2PERIPH_AFIO, ENABLE);
  
  /* NVIC configuration */
  NVIC_Configuration();  
	
  /* Configure the GPIO ports */
  //GPIO_Configuration();
  
  /* Configure the Ethernet peripheral */
  //Ethernet_Configuration();

  /* SystTick configuration: an interrupt every 10ms */
//  RCC_GetClocksFreq(&RCC_Clocks);
	
//  SysTick_Config(RCC_Clocks.SYSCLK_Freq / 100);

  /* Update the SysTick IRQ priority should be higher than the Ethernet IRQ */
  /* The Localtime should be updated during the Ethernet packets processing */
//  NVIC_SetPriority (SysTick_IRQn, 1);  
}

/**
  * @brief  Configure NVIC for ISR
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType   NVIC_InitStructure;

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);   
  
  /* Enable the Ethernet global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);   
    
  NVIC_InitStructure.NVIC_IRQChannel = ETH_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure GPIO for ethernet
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

#if MII_RX_REMAP
  GPIO_PinsRemapConfig(GPIO_Remap_ETH, ENABLE);
#endif
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_GPIOC |
                         RCC_APB2PERIPH_GPIOD | RCC_APB2PERIPH_GPIOE | RCC_APB2PERIPH_AFIO, ENABLE);
  /* ETHERNET pins configuration */
  /* AF Output Push Pull:
    ETH_MII_MDIO / ETH_RMII_MDIO: PA2
    ETH_MII_MDC / ETH_RMII_MDC: PC1  
  */
  /* Configure PA2 as alternate function push-pull MDIO*/
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*PC1-->MDC*/
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
	/*MII Mode GPIO configuration*/
#ifdef MII_MODE
/**********************MII Tx Pin Define****************************/
  /*
	  ETH_MII_TX0-->PB12 AF-PP
	  ETH_MII_TX1-->PB13 AF-PP
		ETH_MII_TX2-->PC2  AF-PP
		ETH_MII_TX3-->PB8  AF-PP
		ETH_MII_TX_EN-->PB11 AF-PP
		ETH_MII_TX_CLK-->PC3
	*/
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8 | GPIO_Pins_11 | GPIO_Pins_12 | GPIO_Pins_13;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2 | GPIO_Pins_3;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	/**********************MII Rx Pin Define****************************/
#if MII_RX_REMAP  /*IO PIN remaped*/
  /*
   ETH_MII_RX_DV-->PD8
   ETH_MII_RXD0-->PD9
   ETH_MII_RXD1-->PD10
   ETH_MII_RXD2-->PD11
   ETH_MII_RXD3-->PD12
	 ETH_MII_RXCLK-->PA1
	 ETH_MII_CRS-->PA0
	 ETH_MII_COL-->PA3
	 ETH_MII_RX_ER-->PB10
  */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10 | GPIO_Pins_11 | GPIO_Pins_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
  /*
   ETH_MII_RX_DV-->PA7
   ETH_MII_RXD0-->PC4
   ETH_MII_RXD1-->PC5
   ETH_MII_RXD2-->PB0
   ETH_MII_RXD3-->PB1
	 ETH_MII_RXCLK-->PA1
	 ETH_MII_CRS-->PA0
	 ETH_MII_COL-->PA3
	 ETH_MII_RX_ER-->PB10
  */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_3 | GPIO_Pins_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4 | GPIO_Pins_5;   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_10;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

#endif  //End MII RX REMAP



#endif //End MII mode


#ifdef RMII_MODE
/**********************RMII Tx Pin Define****************************/
  /*
	  ETH_RMII_TX0-->PB12 AF-PP
	  ETH_RMII_TX1-->PB13 AF-PP
		ETH_RMII_TX_EN-->PB11 AF-PP
	*/
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11 | GPIO_Pins_12 | GPIO_Pins_13;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/**********************RMII Rx Pin Define****************************/
#if MII_RX_REMAP  /*IO PIN remaped*/
  /*
   ETH_RMII_RX_DV-->PD8
   ETH_RMII_RXD0-->PD9
   ETH_RMII_RXD1-->PD10
	 ETH_RMII_REF_CLK-->PA1
  */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
#else
  /*
   ETH_RMII_RX_DV-->PA7
   ETH_RMII_RXD0-->PC4
   ETH_RMII_RXD1-->PC5
	 ETH_RMII_REF_CLK-->PA1
  */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1 | GPIO_Pins_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4 | GPIO_Pins_5;   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

#endif  //End RMII RX REMAP
#endif //End RMII mode


  /* ADC Channel4 config --------------------------------------------------------*/
  /* Configure PA4(ADC Channel4) as analog input -------------------------*/
  #ifdef ADC_NECESSARY
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  #endif

  /* MCO pin configuration------------------------------------------------- */
  /* Configure MCO (PA8) as alternate function push-pull */
  #ifndef CRYSTAL_ON_PHY
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  #endif	
}

void init_ethernet(void)
{
  ETH_InitType ETH_InitStructure;
  uint32_t i;
  GPIO_Configuration();
  /* MII/RMII Media interface selection ------------------------------------------*/
#ifdef MII_MODE /* Mode MII with AT32F407-EVAL  */
  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

  /* Get 25MHz from system clock 200MHz on PA8 pin (MCO) */
  #ifndef CRYSTAL_ON_PHY
  RCC_CLKOUTConfig(RCC_CLKOUT_SYSCLK, RCC_MCOPRE_8);
  #endif

#elif defined RMII_MODE  /* Mode RMII with AT32F407-EVAL */
  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);
  #ifndef CRYSTAL_ON_PHY
	RCC_CLKOUTConfig(RCC_CLKOUT_SYSCLK, RCC_MCOPRE_8); /*25M to RMII Mode*/
  #endif
#endif
	
	/*Reset phy*/
  Reset_Phy();
  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();

  /* Software reset */
  ETH_SoftwareReset();

  /* Wait for software reset */
//  while (ETH_GetSoftwareResetStatus() == SET);
	for(i=0;i<ETH_TOUT;i++)
	{
		if(ETH_GetSoftwareResetStatus()!=SET)
			break;
	}

  /* ETHERNET Configuration ------------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;//ETH_AutoNegotiation_Enable  ;
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Enable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;//ETH_DropTCPIPChecksumErrorFrame_Enable; 
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;                                                          
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;                                                                 
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
  ETH_Init(&ETH_InitStructure, PHY_ADDRESS);

  /* Set the Ethernet MAC Address registers */
  ETH->MACA0H = ((uint32_t)own_hw_adr[5] <<  8) | (uint32_t)own_hw_adr[4];
  ETH->MACA0L = ((uint32_t)own_hw_adr[3] << 24) | (uint32_t)own_hw_adr[2] << 16 |
                 ((uint32_t)own_hw_adr[1] <<  8) | (uint32_t)own_hw_adr[0];

  /* Initialize Tx and Rx DMA Descriptors */
  rx_descr_init ();
	tx_descr_init ();
  
  /* Enable ETH transmition and recetion */
  ETH_Start();
  /* Enable the Ethernet Rx Interrupt */
  ETH_DMAITConfig(ETH_DMA_INT_NIS | ETH_DMA_INT_AIS | ETH_DMA_INT_RBU | ETH_DMA_INT_R, ENABLE);  
}

/**
  * @brief  reset the phy
  * @param  None
  * @retval None
  */
void static Reset_Phy(void)
{
	GPIO_InitType GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC, GPIO_Pins_8);
	Delay_ms(2);
	GPIO_SetBits(GPIOC, GPIO_Pins_8);
	Delay_ms(2);
}

static void rx_descr_init (void) 
{
	U32 i,next;

	/* Initialize Receive DMA Descriptor array. */
	RxBufIndex = 0;
	
	for (i = 0, next = 0; i < NUM_RX_BUF; i++)
	{
		if (++next == NUM_RX_BUF) {
      next = 0;
    }
		Rx_Desc[i].Stat = ETH_DMARxDesc_OWN;
		Rx_Desc[i].Ctrl = ETH_DMARxDesc_RCH | ETH_BUF_SIZE;
		Rx_Desc[i].Addr = (U32)&rx_buf[i];
		Rx_Desc[i].Next = (U32)&Rx_Desc[next];
	}
	
	ETH->DMARDLADDR = (U32)&Rx_Desc[0];
}

static void tx_descr_init (void) 
{
	U32 i,next;

	/* Initialize Transmit DMA Descriptor array. */
	TxBufIndex = 0;
	for (i = 0, next = 0; i < NUM_TX_BUF; i++)
	{
		if (++next == NUM_TX_BUF) {
      next = 0;
    }
		Tx_Desc[i].CtrlStat = ETH_DMATxDesc_TCH | ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;
		Tx_Desc[i].Addr     = (U32)&tx_buf[i];
		Tx_Desc[i].Next     = (U32)&Tx_Desc[next];
	}
	
	ETH->DMATDLADDR = (U32)&Tx_Desc[0];
}

void send_frame (OS_FRAME *frame) 
{
  /* Send frame to ETH ethernet controller */
	U32 *sp,*dp;
	U32 i,j;

	j = TxBufIndex;
	
	/* Wait until previous packet transmitted. */
//	while (Tx_Desc[j].CtrlStat & ETH_DMATxDesc_OWN);
	for(i=0;i<ETH_TOUT;i++)
	{
		if((Tx_Desc[j].CtrlStat & ETH_DMATxDesc_OWN)==0)
			break;
	}

	sp = (U32 *)&frame->data[0];
	dp = (U32 *)(Tx_Desc[j].Addr & ~3);

	/* Copy frame data to ETH IO buffer. */
	for (i = (frame->length + 3) >> 2; i; i--) 
	{
		*dp++ = *sp++;
	}
	
	Tx_Desc[j].Size      = frame->length;	
	Tx_Desc[j].CtrlStat |= ETH_DMATxDesc_OWN;
	
	if (++j == NUM_TX_BUF) j = 0;
	TxBufIndex = j;
	
	/* Start frame transmission. */
	ETH->DMASTS   = ETH_DMA_FLAG_TPS;	
	ETH->DMATPD = 0;
}

void ETH_IRQHandler (void) 
{
	OS_FRAME *frame;
	U32 i, RxLen,ethcnt=0;
	U32 *sp,*dp;

	i = RxBufIndex;
	
//	do 
//	{
	for(ethcnt=0;ethcnt<ETH_TOUT;ethcnt++)
	{
		if (Rx_Desc[i].Stat & DMA_RX_ERROR_MASK) {
			goto rel;
		}
		if ((Rx_Desc[i].Stat & DMA_RX_SEG_MASK) != DMA_RX_SEG_MASK) {
			goto rel;
		}		
		RxLen = ((Rx_Desc[i].Stat >> 16) & 0x3FFF) - 4;
		if (RxLen > ETH_MTU) {
			/* Packet too big, ignore it and free buffer. */
			goto rel;
		}		
		/* Flag 0x80000000 to skip sys_error() call when out of memory. */
		frame = alloc_mem (RxLen | 0x80000000);		
		/* if 'alloc_mem()' has failed, ignore this packet. */
		if (frame != NULL) 
		{
			sp = (U32 *)(Rx_Desc[i].Addr & ~3);
			dp = (U32 *)&frame->data[0];
			for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
				*dp++ = *sp++;
			}
			put_in_queue (frame);
		}
		
		/* Release this frame from ETH IO buffer. */
rel: Rx_Desc[i].Stat = ETH_DMARxDesc_OWN;

		if (++i >= NUM_RX_BUF) i = 0;
		if((Rx_Desc[i].Stat & ETH_DMARxDesc_OWN)==0)
			break;
	}

//	while (!(Rx_Desc[i].Stat & ETH_DMARxDesc_OWN));
	
	RxBufIndex = i;

	if (ETH->DMASTS & ETH_DMA_INT_RBU) 
	{
		/* Receive buffer unavailable, resume DMA */
		ETH->DMASTS = ETH_DMA_FLAG_RBU;
		ETH->DMARPD = 0;
	}
	
	/* Clear pending interrupt bits */
  ETH_DMAClearITPendingBit(ETH_DMA_INT_NIS);
  ETH_DMAClearITPendingBit(ETH_DMA_INT_AIS);
  ETH_DMAClearITPendingBit(ETH_DMA_INT_R);
	//ETH->DMASTS = ETH_DMA_FLAG_NIS | ETH_DMA_FLAG_AIS | ETH_DMA_FLAG_R;
}

void int_enable_eth (void) {
    /* Ethernet Interrupt Enable function. */
    NVIC->ISER[2] = 1 << 14;
}
void int_disable_eth (void) {
    /* Ethernet Interrupt Disable function. */
    NVIC->ICER[2] = 1 << 14;
}
