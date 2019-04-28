# [STM32F429I](https://github.com/sochub/STM32F429I) 
[![sites](SoC/SoC.png)](http://www.qitas.cn) 
####  qitas@qitas.cn
#### 归属：[cortex M4](https://github.com/sochub/CM4) 
#### 父级：[STM32F4](https://github.com/sochub/STM32) 

### [简介](https://github.com/sochub/STM32F429I/wiki)

高性能嵌入式处理器，具有丰富的外设和较高的处理能力，广泛应用于各种控制场景和原型验证。

#### system

* Cortex®-M4 CPU with FPU,frequency up to 180 MHz, MPU, 225 DMIPS/1.25 DMIPS/MHz (Dhrystone 2.1)
* Up to 2 MB of Flash memory organized into two banks allowing read-while-write
* Up to 256+4 KB of SRAM including 64-KB of CCM (core coupled memory) data RAM
* 1.7 V to 3.6 V application supply and I/Os; POR, PDR, PVD and BOR
* 4-to-26 MHz crystal oscillator
* Internal 16 MHz factory-trimmed RC (1% accuracy)
* 32 kHz oscillator for RTC with calibration
* VBAT supply for RTC, 20×32 bit backup registers + optional 4 KB backup SRAM
* True random number generator
* CRC calculation unit
* RTC: subsecond accuracy, hardware calendar


#### interface

* LCD parallel interface: 8080/6800 modes (width up to 4096 pixels,height up to 2048 lines and pixel clock up to 83 MHz)
* 8- to 14-bit parallel camera interface up to 54 Mbytes/s
* 3×12-bit, 2.4 MSPS ADC: up to 24 channels and 7.2 MSPS in triple interleaved mode
* 2×12-bit D/A converters
* DMA: 16-stream DMA controller with FIFOs and burst support
* Up to 17 timers: up to twelve 16-bit and two 32-bit timers up to 180 MHz, each with up to 4 IC/OC/PWM
* Up to 168 I/O ports with interrupt capability
* Up to 164 fast I/Os up to 90 MHz
* Up to 166 5 V-tolerant I/Os

#### connectivity

* Up to 3 × I2C interfaces (SMBus/PMBus)
* Up to 4 USARTs/4 UARTs (11.25 Mbit/s, ISO7816 interface, LIN, IrDA, modem control)
* Up to 6 SPIs (45 Mbits/s), 2 with muxed full-duplex I2S for audio class accuracy via internal audio PLL or external clock
* 1 x SAI (serial audio interface)
* 2 × CAN (2.0B Active) and SDIO interface
* USB 2.0 full-speed device/host/OTG controller with on-chip PHY
* USB 2.0 high-speed/full-speed device/host/OTG controller with dedicated DMA, on-chip full-speed PHY and ULPI
* 10/100 Ethernet MAC with dedicated DMA: supports IEEE 1588v2 hardware, MII/RMII



##  [SoC开发平台](http://www.qitas.cn)  


