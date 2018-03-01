#include "type.h"
#include "debug.h"
#include "media_access.h"
#include "sd.h"
#include "spi.h"
							
extern char ascii_string[16];
extern CARDCONFIG CardConfig;

/* Local variables */
static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive number (0) */
)
{

	if (drv) return STA_NOINIT;			/* Supports only single drive */
//	if (Stat & STA_NODISK) return Stat;	/* No card in the socket */

	if (SD_Init() && SD_ReadConfiguration())
		Stat &= ~STA_NOINIT;

	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, *ptr = buff;

	if (drv) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;

	switch (ctrl) {
	case CTRL_SYNC :		/* Make sure that no pending write process */
        SD_Select();
		if (SD_WaitForReady() == SD_TRUE)
			res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
		*(DWORD*)buff = CardConfig.sectorcnt;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
		*(WORD*)buff = CardConfig.sectorsize;	//512;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		*(DWORD*)buff = CardConfig.blocksize;
		res = RES_OK;
		break;

	case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
		*ptr = CardType;
		res = RES_OK;
		break;

	case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
		for (n=0;n<16;n++)
			*(ptr+n) = CardConfig.csd[n]; 
		res = RES_OK;
		break;

	case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
		for (n=0;n<16;n++)
			*(ptr+n) = CardConfig.cid[n];
		res = RES_OK;
		break;

	case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
		for (n=0;n<4;n++)
			*(ptr+n) = CardConfig.ocr[n];
		res = RES_OK;
		break;

	case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
		for (n=0;n<64;n++)
            *(ptr+n) = CardConfig.status[n]; 
        res = RES_OK;   
		break;

	default:
		res = RES_PARERR;
	}

    SD_DeSelect();

	return res;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE drv,			/* Physical drive number (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (SD_ReadSector (sector, buff, count) == SD_TRUE)	
		return RES_OK;
	else
		return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */

	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
//	if (Stat & STA_PROTECT) return RES_WRPRT;

	if ( SD_WriteSector(sector, buff, count) == SD_TRUE)
		return RES_OK;
	else
		return 	RES_ERROR;
}
#endif /* _READONLY == 0 */


DWORD get_fattime (void)
{
   	DWORD time = 0;	
   	DWORD month = 0; 
   	DWORD day = 0; 
   	DWORD hour = 0; 
   	DWORD minute = 0;
	static DWORD second = 0;
   	
   	char tp0[]="\n\r Inside get_fattime()";
	char byte = 0;
   	
	printString(tp0);
  // bit31:25 Year origin from the 1980 (0..127, e.g. 37 for 2017)
    time = (0x11 << 25);
  //bit24:21 Month (1..12)
  	month = 0x01;
	month =  month << 21;
	time = time | month;
  //bit20:16 Day of the month(1..31)
	day = 0x01;
	day = day << 16;
	time = time | day;
  //bit15:11 Hour (0..23)
    hour = 0x01;
	hour = hour << 11;
	time = time | hour; 
  //bit10:5 Minute (0..59)
    minute = 0x01;
	minute = minute << 5;
	time = time | minute; 
  //bit4:0 Second / 2 (0..29, e.g. 25 for 50)	
    time = time | second;
	second = second + 2;
	byte = (char)second;
	printChar_BCD(byte);
    return(time); 
}

/*-----------------------------------------------------------------------*/
/* Device timer function  (Platform dependent)                           */
/*-----------------------------------------------------------------------*/
/* This function must be called from timer interrupt routine in period
/  of 10 ms to generate card control timing.
*/
//void disk_timerproc (void)
//{
//    WORD n;

//	n = Timer1;						/* 100Hz decrement timer stopped at 0 */
//	if (n) Timer1 = --n;
//	n = Timer2;
//	if (n) Timer2 = --n;               
//}
