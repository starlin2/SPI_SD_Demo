///////////////////////////////////////////////////
//
//	FILE:       MPC82G516_SPI_Demo.c
//	AUTHOR:     Neo (Hsin-Chih Lin)
//	COPYRIGHT:  (c) Megawin Technology Co., Ltd.
//	CREATED:    2007.11.19
//	PURPOSE:    Demo SPI SD Card Function
//
///////////////////////////////////////////////////

//#define DEBUG
#ifdef DEBUG
#include "debug.h"
#endif
//#define TEST
#include "REG_MPC82G516.H"
#include "SD.H"
#include "fat32.h"
unsigned char xdata SD_buffer[1024] = {0};               // Buffer for SD i/o for data and registers
unsigned char status = 0;
extern unsigned long Data_Begin;


void Init_Uart(void)
{
    TMOD=0x02;                                  //Set Timer0 Mode 2
    IE=0x82;                                    //EA Timer 0
    AUXIE=0x10;                                 //2UART interrupt
    TH0=0;                                      //Set Timer1 overflow rate
    TL0=0;
//Uart2
    S2CON=0x50;                                 //Set UART Mode 1
    S2BRT=0xfa;                                 //Set 2UART overflow rate
    AUXR2=0x0c;                                 //SMOD = 1,S2TX12 = 1
    AUXR2|=0x10;                                //S2TR = 1
}
unsigned int t_count=0;
void Timer0_Interrupt() interrupt 1
{
    t_count++;
}
unsigned int rx_count=0;
void UART2_Interrupt() interrupt 12
{
    if(t_count == 0)
        TR0=1;
    if(S2CON&0x01)
    {                   //check if interrupt caused by S2RI ?
        S2CON&=0xFE;    //clear S2RI
        SD_buffer[rx_count++]=S2BUF;//Send data to RS232 from UART_2
    }
}
unsigned long data_count = 0;
void main( void )
{
    unsigned int i = 0;
    unsigned char j = 0;
#ifdef DEBUG
    Debug_clear_LCD();
#endif
#ifdef TEST
    initSD();
    while(1){}
#endif    
#if 0//write test (clear SD card)
    initSD();
    for (j = 0; j < 13; j++) 
    {
        for (i = 0; i < 512; i++) 
            SD_buffer[i]=i;
        SDWriteBlock(j, SD_buffer);
    }
    while (1){}
#endif
#if 1//read test
    initSD();
    for (j = 0; j < 15; j++) 
    {
        SDReadBlock(j, SD_buffer);
//        for (i = 0; i < 512; i++) 
//            Debug_show_msg_to_PC(SD_buffer[i]);
    }
    while (1){}
#endif
    Init_Uart();

    while (1)
    {
        switch(status)
        {
            case 0://idel
                if(P06 == 0)//Insert SD card
                    status = 1;
                break;
            case 1:
//Initialisation of the MMC/SD-card
                if(initSD() == 0)//return 0 os success
                {
                    FAT32_Init();
                    status = 2;
                }
                break;
            case 2:
                if(t_count > 2000)// 0.5 ~ 1 sec
                {
                    TR0 = 0;      // stop timer 0 count
                    t_count = 0;
//                    Debug_show_msg_to_LCD(rx_count>>8);
//                    Debug_show_msg_to_LCD(rx_count);
                    status = 3;
                    rx_count = 0;
                }
                break;
            case 3:
                data_count++;
                //if 64MB Data_Begin is 1944
                SDWriteBlock (Data_Begin + data_count*2, SD_buffer);
                SDWriteBlock (Data_Begin + data_count*2+1, SD_buffer+512);
                SD_buffer[0]=data_count>>8;
                SD_buffer[1]=data_count;
                for(i=2;i<512;i++)
                    SD_buffer[i] = 0;
                SDWriteBlock (1, SD_buffer);
                for(i=0;i<1024;i++)
                    SD_buffer[i] = 0;//clear buffer
                status = 2;
                //if 64MB total sector is 124160
                if((Data_Begin + data_count)>Get_total_sector())
                    status =4;
                break;
            case 4:
                //do nothing
                break;
        }
    }
}

