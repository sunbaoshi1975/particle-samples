#ifndef __nRF24L01_API_H
#define __nRF24L01_API_H

#include "nRF24L01.h"
#define uchar unsigned char
#define uint unsigned int

void delay_us(uchar num);
void delay_150us(void);
void Delay_ms(int nCount);
void Delay_us(int nCount);
uchar SPI_RW(uchar byte);
uchar NRF24L01_Write_Reg(uchar reg,uchar value);
uchar NRF24L01_Read_Reg(uchar reg);
uchar NRF24L01_Read_Buf(uchar reg,uchar *pBuf,uchar len);
uchar NRF24L01_Write_Buf(uchar reg, uchar *pBuf, uchar len);
uchar NRF24L01_RxPacket(uchar *rxbuf);
uchar NRF24L01_TxPacket(uchar *txbuf);
uchar NRF24L01_Check(void);
void NRF24L01_RT_Init(void);
void nRF_SENDTIP_RXMODE(uchar *buf);
void nRF_TXMODE(uchar *buf);

#endif
