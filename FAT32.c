#include "sd.H"
#include "FAT32.H"
#include "FAT32_definitions.h"

extern unsigned char xdata SD_buffer[1024];

unsigned long Data_Begin;

void FAT32_Init(void)
{
	unsigned char Number_of_FATS;
    unsigned int Fat_Sectors;
	unsigned int Reserved_Sectors;
	unsigned long Fat_begin_lba;

	if (SDReadBlock(0, SD_buffer))//SD Card LBA is 0
		return ;

		// Make sure there are 512 bytes per cluster
	if (GET_16BIT_WORD(SD_buffer, 0x0B)!=0x200) 
		return ;

		// Load Parameters of FAT32	 
	Number_of_FATS = SD_buffer[BPB_NumFATs];
        // 02
	Reserved_Sectors = GET_16BIT_WORD(SD_buffer, BPB_RsvdSecCnt);
        // 0x22 = 34
	Fat_Sectors = GET_32BIT_WORD(SD_buffer, BPB_FAT32_FATSz32);
        // 0x03bb = 955
	Fat_begin_lba = 0/*LBA_BEGIN*/ + Reserved_Sectors;
        //34
	// The address of the first data cluster on this volume
	Data_Begin = Fat_begin_lba + (Number_of_FATS * Fat_Sectors);
        // 0x0798 = 1944
}
unsigned long Get_total_sector(void)
{
	if (SDReadBlock(0, SD_buffer))//SD Card LBA is 0
		return 0;
	return GET_32BIT_WORD(SD_buffer, BPB_TotSec32);
}
