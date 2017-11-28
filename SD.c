#include "SD.H"
#include "REG_MPC82G516.H"
#include "SD_define.h"
//#define DEBUG
#ifdef DEBUG
#include "debug.h"
#endif

char SDGetResponse(void);
unsigned char SDGetXXResponse(const char resp);
char SDCheckBusy(void);
unsigned char spiSendByte(const unsigned char dat);
//---------------------------------------------------------------------
void SDSendCmd (const char cmd, unsigned long dat, const char crc)
{
  char frame[6];
  char temp;
  char i;
  frame[0]=(cmd|0x40);
  for(i=3;i>=0;i--){
    temp=(char)(dat>>(8*i));
    frame[4-i]=(temp);
  }
  frame[5]=(crc);
  for(i=0;i<6;i++)
    spiSendByte(frame[i]);
}
//--------------- set blocklength 2^n ------------------------------------------------------
char SDSetBlockLength (const unsigned long blocklength)
{
  CS_LOW ();
  // Set the block length to read
  //SD_SET_BLOCKLEN =CMD16
  SDSendCmd(SD_SET_BLOCKLEN, blocklength, 0xFF);

  // get response from SD - make sure that its 0x00 (R1 ok response format)
  if(SDGetResponse()!=0x00)
  { initSD();
    SDSendCmd(SD_SET_BLOCKLEN, blocklength, 0xFF);
    SDGetResponse();
  }

  CS_HIGH ();

  // Send 8 Clock pulses of delay.
  spiSendByte(0xff);

  return SD_SUCCESS;
} // Set block_length


unsigned char spiSendByte(const unsigned char dat)
{
	SPDAT = dat;
	while(SPSTAT != 0x80){}
	SPSTAT = 0x80;
	return SPDAT;
}

char SD_GoIdle()
{
  char response=0x01;
//  CS_LOW();

  //Send Command 0 to put SD in SPI mode
  SDSendCmd(SD_GO_IDLE_STATE,0,0x95);
  //Now wait for READY RESPONSE

#ifdef DEBUG
  Debug_show_msg_to_LCD(SDGetResponse());
    P12=0;
  while(1){}
#endif
  if(SDGetResponse()!=0x01)
    return SD_INIT_ERROR;
  while(response==0x01)
  {
    CS_HIGH();
    spiSendByte(0xff);
    CS_LOW();
    SDSendCmd(SD_SEND_OP_COND,0x00,0xff);
    response=SDGetResponse();
  }
  CS_HIGH();
  spiSendByte(0xff);
  return (SD_SUCCESS);
}
// Initialize SD card
char initSD (void)
{

  //raise SS and MOSI for 80 clock cycles
  //SendByte(0xff) 10 times with SS high
  //RAISE SS
  char i;

//Init SPI
  SPCTL = 0xd0;				//Enable SPI Master
//  initSPI();
  IFADRL = 0x01;
  if((SCMD & 0xf0)!=0xf0)
    return SD_INIT_ERROR;
  //initialization sequence on PowerUp
//  CS_LOW();
  for(i=0;i<=9;i++)
    spiSendByte(0xff);
//  CS_HIGH();

  return (SD_GoIdle());
}

// SD Get Responce
char SDGetResponse(void)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  char i=0;

  char response;

  while(i<=64)
  {
    response=spiSendByte(0xff);
    if(response==0x00)break;
    if(response==0x01)break;
    i++;
  }
  return response;
}

unsigned char SDGetXXResponse(const unsigned char resp)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  unsigned int i=0;

  unsigned char response;

  while(i<=1000)
  {
    response=spiSendByte(0xff);
    if(response==resp)break;
    i++;
  }
  return response;
}
char SDCheckBusy(void)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  char i=0;

  char response;
  char rvalue;
  while(i<=64)
  {
    response=spiSendByte(0xff);
    response &= 0x1f;
    switch(response)
    {
      case 0x05: rvalue=SD_SUCCESS;break;
      case 0x0b: return(SD_CRC_ERROR);
      case 0x0d: return(SD_WRITE_ERROR);
      default:
        rvalue = SD_OTHER_ERROR;
        break;
    }
    if(rvalue==SD_SUCCESS)break;
    i++;
  }
  i=0;
  do
  {
    response=spiSendByte(0xff);
    i++;
  }while(response==0);
  return response;
}

char SDReadBlock(const unsigned int sector, unsigned char *pBuffer)
{
    unsigned int i = 0;
    char rvalue = SD_RESPONSE_ERROR;
    unsigned long address = (unsigned long)sector * 512;
  // Set the block length to read
  if (SDSetBlockLength (512) == SD_SUCCESS)   // block length could be set
  {
    CS_LOW ();
    // send read command SD_READ_SINGLE_BLOCK=CMD17
    SDSendCmd (SD_READ_SINGLE_BLOCK,address, 0xFF);
    // Send 8 Clock pulses of delay, check if the SD acknowledged the read block command
    // it will do this by sending an affirmative response
    // in the R1 format (0x00 is no errors)
    if (SDGetResponse() == 0x00)
    {
      // now look for the data token to signify the start of
      // the data
      if (SDGetXXResponse(SD_START_DATA_BLOCK_TOKEN) == SD_START_DATA_BLOCK_TOKEN)
      {
        // clock the actual data transfer and receive the bytes; spi_read automatically finds the Data Block
        for (i = 0; i < 512; i++)
          pBuffer[i] = spiSendByte(0xff);   // is executed with card inserted
        // get CRC bytes (not really needed by us, but required by SD)
        spiSendByte(0xff);
        spiSendByte(0xff);
        rvalue = SD_SUCCESS;
      }
      else
      {
        // the data token was never received
        rvalue = SD_DATA_TOKEN_ERROR;      // 3
      }
    }
    else
    {
      // the SD never acknowledge the read command
      rvalue = SD_RESPONSE_ERROR;          // 2
    }
  }
  else
  {
    rvalue = SD_BLOCK_SET_ERROR;           // 1
  }
  CS_HIGH ();
  spiSendByte(0xff);
  return rvalue;
}// SD_read_block



//---------------------------------------------------------------------
char SDWriteBlock (const unsigned int sector,unsigned char *pBuffer)
{
  unsigned int i = 0;
  char rvalue = SD_RESPONSE_ERROR;         // SD_SUCCESS;
    unsigned long address = (unsigned long)sector * 512;

  // Set the block length to read
  if (SDSetBlockLength (512) == SD_SUCCESS)   // block length could be set
  {
    CS_LOW ();
    // send write command
    SDSendCmd (SD_WRITE_BLOCK,address, 0xFF);

    // check if the SD acknowledged the write block command
    // it will do this by sending an affirmative response
    // in the R1 format (0x00 is no errors)
    if (SDGetXXResponse(SD_R1_RESPONSE) == SD_R1_RESPONSE)
    {
      spiSendByte(0xff);
      // send the data token to signify the start of the data
      spiSendByte(0xfe);
      // clock the actual data transfer and transmitt the bytes
      for (i = 0; i < 512; i++)
        spiSendByte(pBuffer[i]);            
      // put CRC bytes (not really needed by us, but required by SD)
      spiSendByte(0xff);
      spiSendByte(0xff);
      // read the data response xxx0<status>1 : status 010: Data accected, status 101: Data
      //   rejected due to a crc error, status 110: Data rejected due to a Write error.
      SDCheckBusy();
      rvalue = SD_SUCCESS;
    }
    else
    {
      // the SD never acknowledge the write command
      rvalue = SD_RESPONSE_ERROR;   // 2
    }
  }
  else
  {
    rvalue = SD_BLOCK_SET_ERROR;   // 1
  }
  // give the SD the required clocks to finish up what ever it needs to do
  //  for (i = 0; i < 9; ++i)
  //    spiSendByte(0xff);

  CS_HIGH ();
  // Send 8 Clock pulses of delay.
  spiSendByte(0xff);
  return rvalue;
} // SD_write_block
