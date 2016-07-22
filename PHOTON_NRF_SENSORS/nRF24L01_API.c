#include "nRF24L01_API.h"
#include "nRF24L01.h"
#include "main.h"

#define TX_ADR_WIDTH    5     
#define RX_ADR_WIDTH    5     
#define TX_PLOAD_WIDTH  32    
#define RX_PLOAD_WIDTH  32    

/***  use GPIO simulator the SPI         ***/
//#define NRF_CSN   PFout(10)  
#define NRF_CSN_H()       GPIO_SetBits(GPIOC,GPIO_Pin_2) 
#define NRF_CSN_L()       GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define NRF_IRQ()         GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)

//#define NRF_IRQ   PGin(15)  
#define NRF_SCK_H()       GPIO_SetBits(SPIx_SCK_GPIO_PORT,SPIx_SCK_PIN)
#define NRF_SCK_L()       GPIO_ResetBits(SPIx_SCK_GPIO_PORT,SPIx_SCK_PIN)

#define NRF_CE_H()        GPIO_SetBits(GPIOC,GPIO_Pin_5)
#define NRF_CE_L()        GPIO_ResetBits(GPIOC,GPIO_Pin_5)
//#define NRF_CE   	PCout(5)  
#define NRF_MOSI_H()      GPIO_SetBits(SPIx_MOSI_GPIO_PORT,SPIx_MOSI_PIN)
#define NRF_MOSI_L()      GPIO_ResetBits(SPIx_MOSI_GPIO_PORT,SPIx_MOSI_PIN)
//#define NRF_MOSI  PBout(15)  

#define NRF_MISO()        GPIO_ReadInputDataBit(SPIx_MISO_GPIO_PORT,SPIx_MISO_PIN)
const uchar TX_ADDRESS[TX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; 
const uchar RX_ADDRESS[RX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; 

void delay_us(uchar num)
{
	uchar i,j; 
	for(i=0;i>num;i++)
 	for(j=100;j>0;j--);
}
void delay_150us(void)
{
	uint i;
	for(i=0;i>600;i++);
}

void Delay_us(int nCount){    //5us
 __IO uint32_t tmp = 12*nCount;
  for(tmp; tmp !=0; tmp--);
}

void Delay_ms(int nCount){     //1ms
 __IO uint32_t tmp = 12500*nCount;
  for(tmp; tmp !=0; tmp--);
}

uchar SPI_RW(uchar byte)
{
	uchar bit_ctr;
	for(bit_ctr=0;bit_ctr<8;bit_ctr++)  // Êä³ö8Î»
	{
		if((uchar)(byte&0x80)==0x80)
		NRF_MOSI_H(); 			// MSB TO MOSI
		else 
		NRF_MOSI_L(); 
		byte=(byte<<1);					// shift next bit to MSB
		NRF_SCK_H();
		byte|=NRF_MISO();	        		// capture current MISO bit
		NRF_SCK_L();
	}
	return byte;
}

uchar NRF24L01_Write_Reg(uchar reg,uchar value)
{
	uchar status;

	NRF_CSN_L();                     
  	status = SPI_RW(reg);		
	SPI_RW(value);
	NRF_CSN_H();                 

	return status;
}

uchar NRF24L01_Read_Reg(uchar reg)
{
 	uchar value;

	NRF_CSN_L();            
  	SPI_RW(reg);			
	value = SPI_RW(NOP);
	NRF_CSN_H();             	

	return value;
}

uchar NRF24L01_Read_Buf(uchar reg,uchar *pBuf,uchar len)
{
	uchar status,u8_ctr;
	NRF_CSN_L();                   	     
	status=SPI_RW(reg);				   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)
	pBuf[u8_ctr]=SPI_RW(0XFF);		
	NRF_CSN_H();                		
	return status;        			
}

uchar NRF24L01_Write_Buf(uchar reg, uchar *pBuf, uchar len)
{
	uchar status,u8_ctr;
	NRF_CSN_L();
	status = SPI_RW(reg);			
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)
	SPI_RW(*pBuf++); 				
	NRF_CSN_H();
  return status;          		
}							  					   


uchar NRF24L01_RxPacket(uchar *rxbuf)
{
	uchar state;
	state=NRF24L01_Read_Reg(STATUS);  			   	 
	NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS,state); 
	if(state&RX_OK)								
	{
		NRF_CE_L();
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);
		NRF24L01_Write_Reg(FLUSH_RX,0xff);					
		NRF_CE_H();
		Delay_us(20); 
		return 0; 
	}	   
	return 1;
}

uchar NRF24L01_TxPacket(uchar *txbuf)
{
	uchar state;
   
	NRF_CE_L();												
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);	
 	NRF_CE_H();												   
	while(NRF_IRQ()==1);									
	state=NRF24L01_Read_Reg(STATUS);  							   
	NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS,state); 			
	if(state&MAX_TX)										
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);					
		return MAX_TX; 
	}
	if(state&TX_OK)											
	{
		return TX_OK;
	}
	return 0xff;											
}

  
uchar NRF24L01_Check(void)
{
	uchar check_in_buf[5]={0x11,0x22,0x33,0x44,0x55};
	uchar check_out_buf[5]={0x00};

	NRF_SCK_L();
	NRF_CSN_H();   
	NRF_CE_L();

	NRF24L01_Write_Buf(nRF_WRITE_REG+TX_ADDR, check_in_buf, 5);

	NRF24L01_Read_Buf(nRF_READ_REG+TX_ADDR, check_out_buf, 5);

	if((check_out_buf[0] == 0x11)&&\
	   (check_out_buf[1] == 0x22)&&\
	   (check_out_buf[2] == 0x33)&&\
	   (check_out_buf[3] == 0x44)&&\
	   (check_out_buf[4] == 0x55))return 0;
	else return 1;
}			


void NRF24L01_RT_Init(void)
{	
	NRF_CE_L();		  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
	NRF24L01_Write_Reg(FLUSH_RX,0xff);									    
  	NRF24L01_Write_Buf(nRF_WRITE_REG+TX_ADDR,(uchar*)TX_ADDRESS,TX_ADR_WIDTH); 
  	NRF24L01_Write_Buf(nRF_WRITE_REG+RX_ADDR_P0,(uchar*)RX_ADDRESS,RX_ADR_WIDTH); 	  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+EN_AA,0x01);         
  	NRF24L01_Write_Reg(nRF_WRITE_REG+EN_RXADDR,0x01);   
  	NRF24L01_Write_Reg(nRF_WRITE_REG+SETUP_RETR,0x1a);
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RF_CH,0);        // freq=2.4+0GHz
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RF_SETUP,0x0F);   
  	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0f);    
	NRF_CE_H();									 
}

void nRF_SENDTIP_RXMODE(uchar *buf)
{
	NRF_CE_L();
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0e);
	NRF_CE_H();
	delay_us(15);
	NRF24L01_TxPacket(buf);
	NRF_CE_L();
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG, 0x0f);
	NRF_CE_H();	
}

void nRF_TXMODE(uchar *buf)
{
        NRF_CE_L();
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0e);
	NRF_CE_H();
	delay_us(15);
	NRF24L01_TxPacket(buf);



}