#ifndef _MEDIA_ACCESS_H_
#define _MEDIA_ACCESS_H_


#include "diskio.h"

DSTATUS disk_initialize (BYTE drv);
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
);

DRESULT disk_read (
	BYTE drv,			/* Physical drive number (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
);

DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0) */
);

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
);
#endif /* _READONLY == 0 */


DWORD get_fattime (void);

//void disk_timerproc (void)


#endif //_MEDIA_ACCESS_H_
