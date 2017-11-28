// *************************************************************************************
//
// Filename:  SD.h: 
//
// *************************************************************************************
// SD init
char initSD (void);

// read a size Byte big block beginning at the address.
char SDReadBlock(const unsigned int sector, unsigned char *pBuffer);
//char SDReadBlock(const unsigned long address, const unsigned long count, unsigned char *pBuffer);

// write a 512 Byte big block beginning at the (aligned) address
char SDWriteBlock (const unsigned int sector,unsigned char *pBuffer);
//char SDWriteBlock (const unsigned long address, const unsigned long count, unsigned char *pBuffer);

