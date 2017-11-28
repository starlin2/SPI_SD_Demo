typedef unsigned short UINT16;
typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned long UINT32;

#define GET_32BIT_WORD(buffer, location)	( ((UINT32)buffer[location+3]<<24) + ((UINT32)buffer[location+2]<<16) + ((UINT32)buffer[location+1]<<8) + (UINT32)buffer[location+0] )
#define GET_16BIT_WORD(buffer, location)	( ((UINT16)buffer[location+1]<<8) + (UINT16)buffer[location+0] )

//-----------------------------------------------------------------------------
//			FAT32 Offsets
//		Name				Offset
//-----------------------------------------------------------------------------

// Boot Sector
/*
#define BS_jmpBoot				0	// Length = 3
#define BS_OEMName				3	// Length = 8
#define BPB_BytsPerSec			11	// Length = 2
*/
#define BPB_SecPerClus			13	// Length = 1
#define BPB_RsvdSecCnt			14	// Length = 2
#define BPB_NumFATs				16	// Length = 1
#define BPB_TotSec32			32	// Length = 4
/*
#define BPB_RootEntCnt			17	// Length = 2
#define BPB_TotSec16			19	// Length = 2
#define BPB_Media				21	// Length = 1
#define	BPB_FATSz16				22	// Length = 2
#define BPB_SecPerTrk			24	// Length = 2
#define BPB_NumHeads			26	// Length = 2
#define BPB_HiddSec				28	// Length = 4
#define BPB_TotSec32			32	// Length = 4

// FAT 12/16
#define BS_FAT_DrvNum			36	// Length = 1
#define BS_FAT_BootSig			38	// Length = 1
#define BS_FAT_VolID			39	// Length = 4
#define BS_FAT_VolLab			43	// Length = 11
#define BS_FAT_FilSysType		54	// Length = 8
*/
// FAT 32
#define BPB_FAT32_FATSz32		36	// Length = 4
//#define BPB_FAT32_ExtFlags		40	// Length = 2
//#define BPB_FAT32_FSVer			42	// Length = 2
#define BPB_FAT32_RootClus		44	// Length = 4
//#define BPB_FAT32_FSInfo		48	// Length = 2
//#define BPB_FAT32_BkBootSec		50	// Length = 2
//#define BS_FAT32_DrvNum			64	// Length = 1
//#define BS_FAT32_BootSig		66	// Length = 1
//#define BS_FAT32_VolID			67	// Length = 4
//#define BS_FAT32_VolLab			71	// Length = 11
//#define BS_FAT32_FilSysType		82	// Length = 8

