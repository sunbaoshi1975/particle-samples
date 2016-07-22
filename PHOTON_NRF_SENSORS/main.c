#include "main.h"
#include "nRF24L01_API.h"

#define NRF_IRQ()       GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)

void NVIC_Configuration(void)
{
 NVIC_InitTypeDef NVIC_InitStructure;
  
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/***  use GPIO simulator the SPI         ***/
void ALL_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStructure;

  NVIC_Configuration();

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOB, ENABLE); 
	
  GPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN|SPIx_MOSI_PIN;  //LED GPIO_Pin_2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  GPIO_InitStructure.GPIO_Pin = SPIx_MISO_PIN;   //MISO
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
        
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_5;   //CSN   CE
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;    //NRF IRQ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

 
}

int main(void)
{

	ALL_GPIO_Init();	

	GPIO_SetBits(GPIOA,SPIx_MISO_PIN);
	GPIO_SetBits(GPIOC,GPIO_Pin_3);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
        
	while(NRF24L01_Check()); // check NRF24L01 exist£¬
	NRF24L01_RT_Init();	
	send_buf[1]=0x45;					//E
	send_buf[2]=0x61;					//a
	send_buf[3]=0x73;					//s
	send_buf[4]=0x74;					//t
	send_buf[5]=0x66;					//f
	send_buf[6]=0x69;					//i
	send_buf[7]=0x65;					//e
	send_buf[8]=0x6c;					//l
	send_buf[9]=0x64;					//d
	send_buf[0]=10;	                                        //10 byte total£¬rece_buf[0]must be 10£¡£¡£¡				

        nRF_SENDTIP_RXMODE(send_buf); //send data then change to RXMODE

        //Delay_ms(1000);//DHT11 delay 1s for stable
	while(1)
	{    

          if(NRF_IRQ()==0)	 	// if nrf24l01 get data
          {	
              if(NRF24L01_RxPacket(rece_buf)==0)
                {	
                  //USART_SendDataString((rece_buf+1));
                                
                }      
            }              
         }

}


