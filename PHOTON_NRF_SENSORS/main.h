/**
  ******************************************************************************
  * @file    SPI/SPI_TwoBoards/DataExchangeInterrupt/main.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "stm322xg_eval.h"
#include "stm322xg_eval_ioe.h"

/* Exported typedef -----------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Exported define ------------------------------------------------------------*/
/* Uncomment the line below if you will use the SPI peripheral as a Master */
#define SPI_MASTER  
/* Uncomment the line below if you will use the SPI peripheral as a Slave */
//#define SPI_SLAVE 

/* USER_TIMEOUT value for waiting loops. This timeout is just guarantee that the
   application will not remain stuck if the USART communication is corrupted. 
   You may modify this timeout value depending on CPU frequency and application
   conditions (interrupts routines, number of data to transfer, baudrate, CPU
   frequency...). */ 
#define USER_TIMEOUT                    ((uint32_t)0x64) /* Waiting 1s */

/* SPIx Communication boards Interface */
#define SPIx                           SPI1
#define SPIx_CLK                       RCC_APB2Periph_SPI1
#define SPIx_CLK_INIT                  RCC_APB2PeriphClockCmd
#define SPIx_IRQn                      SPI1_IRQn
#define SPIx_IRQHANDLER                SPI1_IRQHandler

#define SPIx_SCK_PIN                   GPIO_Pin_5
#define SPIx_SCK_GPIO_PORT             GPIOA
#define SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define SPIx_SCK_SOURCE                GPIO_PinSource5
#define SPIx_SCK_AF                    GPIO_AF_SPI1

#define SPIx_MISO_PIN                  GPIO_Pin_6
#define SPIx_MISO_GPIO_PORT            GPIOA
#define SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define SPIx_MISO_SOURCE               GPIO_PinSource6
#define SPIx_MISO_AF                   GPIO_AF_SPI1

#define SPIx_MOSI_PIN                  GPIO_Pin_7
#define SPIx_MOSI_GPIO_PORT            GPIOA
#define SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define SPIx_MOSI_SOURCE               GPIO_PinSource7
#define SPIx_MOSI_AF                   GPIO_AF_SPI1
#if 1
#define SPIx_NSS_PIN                  GPIO_Pin_2
#define SPIx_NSS_GPIO_PORT            GPIOC
#define SPIx_NSS_GPIO_CLK             RCC_AHB1Periph_GPIOC
//#define SPIx_NSS_SOURCE               GPIO_PinSource2
//#define SPIx_NSS_AF                   GPIO_AF_SPI1
#endif


#define TXBUFFERSIZE   (countof(TxBuffer) - 1)
#define RXBUFFERSIZE   TXBUFFERSIZE
/* USART Mode Selected */
#define USART_MODE_TRANSMITTER           0x00
#define USART_MODE_RECEIVER              0x01      

/* USART Transaction Type */
#define USART_TRANSACTIONTYPE_CMD        0x00
#define USART_TRANSACTIONTYPE_DATA       0x01


/* Definition for USARTx resources ********************************************/
#define USARTx                           USART1
#define USARTx_CLK                       RCC_APB2Periph_USART1
#define USARTx_CLK_INIT                  RCC_APB2PeriphClockCmd
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler

#define USARTx_TX_PIN                    GPIO_Pin_9                
#define USARTx_TX_GPIO_PORT              GPIOA                       
#define USARTx_TX_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define USARTx_TX_SOURCE                 GPIO_PinSource9
#define USARTx_TX_AF                     GPIO_AF_USART1

#define USARTx_RX_PIN                    GPIO_Pin_10                
#define USARTx_RX_GPIO_PORT              GPIOA                    
#define USARTx_RX_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define USARTx_RX_SOURCE                 GPIO_PinSource10
#define USARTx_RX_AF                     GPIO_AF_USART1


#define BUTTON_PIN                       GPIO_Pin_8
#define BUTTON_GPIO_PORT                 GPIOE
#define BUTTON_GPIO_CLK    
     
/* Misc definition ************************************************************/
/* Transmit buffer size */
#define TXBUFFERSIZE                     (countof(TxBuffer) - 1)
/* Receive buffer size */
#define RXBUFFERSIZE                     TXBUFFERSIZE

/* Joystick Commands */
#define CMD_RIGHT                        0x55
#define CMD_LEFT                         0xAA
#define CMD_UP                           0x33
#define CMD_DOWN                         0xCC
#define CMD_SEL                          0xFF 
#define CMD_NONE                         0x00
#define CMD_ACK                          0x66 

/* Define numbers of bytes to transmit from TxBuffer */
#define CMD_RIGHT_SIZE                   0x01
#define CMD_LEFT_SIZE                    0x05
#define CMD_UP_SIZE                      0x14
#define CMD_DOWN_SIZE                    0x1E
#define CMD_SEL_SIZE                     TXBUFFERSIZE

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#ifdef SPI_MASTER
 uint8_t GetVar_NbrOfData(void);
#endif
unsigned char Usart1_SendDatArray (u8* pBuf, unsigned char ucLen);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
