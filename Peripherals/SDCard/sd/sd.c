
#include <stdint.h>
#include "debug.h"
#include "type.h"
#include "sd.h"
#include "spi.h"
#include "uart0.h"
#include "diskio.h"


CARDCONFIG CardConfig;

/* Local variables */
static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

/**
  * @brief  Initializes the memory card.
  *
  * @param  None
  * @retval SD_TRUE: Init OK.
  *         SD_FALSE: Init failed. 
  *
  * Note: Refer to the init flow at http://elm-chan.org/docs/mmc/mmc_e.html
  **/
SD_BOOL SD_Init (void)
{
	unsigned int i, j, Timer1;
    uint8_t  r1, buf[4];
	unsigned char tp0[]="\n\rSD_Init() Start";
	unsigned char tp1[]="\n\rSending 80 1's";
	unsigned char tp2[]="\n\rSending CMD0: GO_IDLE_STATE";
	unsigned char tp3[]="\n\rCard enters IDLE state....";
	unsigned char tp4[]="\n\rSending CMD8: SEND_IF_COND";
	unsigned char tp5[]="\n\rSending ACMD41: SEND_OP_COND";
	unsigned char tp6[]="\n\rSending CMD58: READ_OCR";
	unsigned char tp7[]="\n\rSending CMD16: SET_BLOCKLEN";
	unsigned char tp100[]="\n\rSD_Init() End";

	for(j=0;tp0[j];j++)  //transmit a predefined string
    uart_TxChar(tp0[j]);
   	/* Set card type to unknown */
   	CardType = CARDTYPE_UNKNOWN;

	/* Init SPI interface */
 	SPI_Init ();

	/* Before reset, Send at least 74 clocks at low frequency 
   	(between 100kHz and 400kHz) with CS high and DI (MISO) high. */
    SD_DeSelect();
	
	for(j=0;tp1[j];j++)  //transmit a predefined string
    uart_TxChar(tp1[j]);
    
	SPI_ConfigClockRate (SPI_CLOCKRATE_LOW);
    for (i = 0; i < 10; i++) SPI_SendByte (0xFF);


   	/* Send CMD0 with CS low to enter SPI mode and reset the card.
   	The card will enter SPI mode if CS is low during the reception of CMD0. 
   	Since the CMD0 (and CMD8) must be sent as a native command, the CRC field
   	must have a valid value. */
		
	for(j=0;tp2[j];j++)  //transmit a predefined string
    uart_TxChar(tp2[j]);	

	if (SD_SendCommand (GO_IDLE_STATE, 0, NULL, 0) != R1_IN_IDLE_STATE) // CMD0
    {
    	goto  init_end;
    }
	/* Now the card enters IDLE state. */

	for(j=0;tp3[j];j++)  //transmit a predefined string
    	uart_TxChar(tp3[j]);
    		
	for(j=0;tp4[j];j++)  //transmit a predefined string
    	uart_TxChar(tp4[j]);

	/* Card type identification Start ... */
	/* Check the card type, needs around 1000ms */    
    r1 = SD_SendCommand (SEND_IF_COND, 0x1AA, buf, 4);  // CMD8
    if (r1 & 0x80) goto init_end;

	 Timer1 = 1000000; // 1000ms - Mathew added this Hack 
     if (r1 == R1_IN_IDLE_STATE) { /* It's V2.0 or later SD card */
        if (buf[2]!= 0x01 || buf[3]!=0xAA) goto init_end; 

        /* The card is SD V2 and can work at voltage range of 2.7 to 3.6V */
		for(j=0;tp5[j];j++)  //transmit a predefined string
    	uart_TxChar(tp5[j]);
        do {Timer1 --; 
            r1 = SD_SendACommand (SD_SEND_OP_COND, 0x40000000, NULL, 0);  // ACMD41
            if      (r1 == 0x00) break;
            else if (r1 > 0x01)  goto init_end;            
        } while (Timer1);

		for(j=0;tp6[j];j++)  //transmit a predefined string
    	uart_TxChar(tp6[j]);
        if (Timer1 && SD_SendCommand (READ_OCR, 0, buf, 4)==R1_NO_ERROR)  // CMD58
            CardType = (buf[0] & 0x40) ? CARDTYPE_SDV2_HC : CARDTYPE_SDV2_SC;
         
    } else { /* It's Ver1.x SD card or MMC card */

        /* Check if it is SD card */
        if (SD_SendCommand (APP_CMD, 0, NULL, 0) & R1_ILLEGAL_CMD)
        {   
            CardType = CARDTYPE_MMC; 
            while (Timer1 && SD_SendCommand (SEND_OP_COND, 0, NULL, 0));
        }  
        else 
        {   
            CardType = CARDTYPE_SDV1; 
            while (Timer1 && SD_SendACommand (SD_SEND_OP_COND, 0, NULL, 0));
        }

        if (Timer1 == 0) CardType = CARDTYPE_UNKNOWN;
    }

	/* For SDHC or SDXC, block length is fixed to 512 bytes, for others,
    the block length is set to 512 manually. */
    if (CardType == CARDTYPE_MMC ||
        CardType == CARDTYPE_SDV1 ||
        CardType == CARDTYPE_SDV2_SC )
    {
		for(j=0;tp7[j];j++)  //transmit a predefined string
    	uart_TxChar(tp7[j]);
        if (SD_SendCommand (SET_BLOCKLEN, SECTOR_SIZE, NULL, 0) != R1_NO_ERROR)
            CardType = CARDTYPE_UNKNOWN;
    }

init_end:
/**************** Mathew **************/
	for(j=0;tp100[j];j++)  //transmit a predefined string
    	uart_TxChar(tp100[j]);
/**************************************/

   	SD_DeSelect();

    if (CardType == CARDTYPE_UNKNOWN){
       	return (SD_FALSE);
   	}else{     
	/* Init OK. use high speed during data transaction stage. */
    //SPI_ConfigClockRate (SPI_CLOCKRATE_HIGH);	// Mathew - Had to comment this out
												// to get the Lexar card working
     return (SD_TRUE);
    }
}

/**
  * @brief  Send a command and receive a response with specified format. 
  *
  * @param  cmd: Specifies the command index.
  * @param  arg: Specifies the argument.
  * @param  buf: Pointer to byte array to store the response content.
  * @param  len: Specifies the byte number to be received after R1 response.
  * @retval Value below 0x80 is the normal R1 response (0x0 means no error) 
  *         Value above 0x80 is the additional returned status code.
  *             0x81: Card is not ready
  *             0x82: command response time out error
***/
uint8_t SD_SendCommand (uint8_t cmd, uint32_t arg, uint8_t *buf, uint32_t len)
{
    uint32_t r1,i;
    uint8_t crc_stop;
	unsigned char tp3[]="\n\rInside SD_SendCommand return 0x81";
	unsigned char tp4[]="\n\rSending CMD0";	
	unsigned char tp5[]="\n\rReceived Response from SD Card";
	

    /* The CS signal must be kept low during a transaction */
    SD_Select();
	/* Mathew Added this delay */
	delay(50000);
    /* Wait until the card is ready to read (DI signal is High) */
   // if (SD_WaitForReady() == SD_FALSE) return 0x81;
	
    /* Prepare CRC7 + stop bit. For cmd GO_IDLE_STATE and SEND_IF_COND, 
    the CRC7 should be valid, otherwise, the CRC7 will be ignored. */
    if      (cmd == GO_IDLE_STATE)  crc_stop = 0x95; /* valid CRC7 + stop bit */
    else if (cmd == SEND_IF_COND)   crc_stop = 0x87; /* valid CRC7 + stop bit */
    else                            crc_stop = 0x01; /* dummy CRC7 + Stop bit */

   	//for(j=0;tp4[j];j++)  //transmit a predefined string
    //    	uart_TxChar(tp4[j]);
    /* Send 6-byte command with CRC. */
    SPI_SendByte (cmd | 0x40);
    SPI_SendByte (arg >> 24);
    SPI_SendByte (arg >> 16);
    SPI_SendByte (arg >> 8);
    SPI_SendByte (arg);
    SPI_SendByte (crc_stop); /* Valid or dummy CRC plus stop bit */

    /* The command response time (Ncr) is 0 to 8 bytes for SDC, 
    1 to 8 bytes for MMC. */
    for (i = 8; i; i--)
    {
        r1 = SPI_RecvByte ();
        if (r1 != 0xFF)	break;   /* received valid response */
		
    }
    if (i == 0)  return (0x82); /* command response time out error */

    /* Read remaining bytes after R1 response */
    if (buf && len)
    {
        do {
            *buf++ = SPI_RecvByte ();
        } while (--len);
    }

    return (r1);
}

/**
  * @brief  Send an application specific command for SD card 
  *         and receive a response with specified format. 
  *
  * @param  cmd: Specifies the command index.
  * @param  arg: Specifies the argument.
  * @param  buf: Pointer to byte array to store the response content.
  * @param  len: Specifies the byte number to be received after R1 response.
  * @retval Value below 0x80 is the normal R1 response(0x0 means no error)
  *         Value above 0x80 is the additional returned status code.
  *             0x81: Card is not ready
  *             0x82: command response time out error
  *
  * Note: All the application specific commands should be precdeded with APP_CMD
***/
uint8_t SD_SendACommand (uint8_t cmd, uint32_t arg, uint8_t *buf, uint32_t len)
{
    uint8_t r1;

    /* Send APP_CMD (CMD55) first */
	r1 = SD_SendCommand(APP_CMD, 0, NULL, 0);
	if (r1 > 1) return r1;    
    
    return (SD_SendCommand (cmd, arg, buf, len));
}


/**
  * @brief  Read card configuration and fill structure CardConfig.
  *
  * @param  None
  * @retval SD_TRUE or SD_FALSE. 
***/
SD_BOOL SD_ReadConfiguration ()
{
    uint8_t buf[16], byte;
    uint32_t i,j, c_size, c_size_mult, read_bl_len;
    SD_BOOL retv;

	unsigned char tp20[]="\n\n\rSD_ReadConfiguration() Start";
	unsigned char tp21[]="\n\rSending CMD58: READ_OCR";
	unsigned char tp22[]="\n\rResponse to CMD58: READ_OCR: ";
	unsigned char tp23[]="\n\rSending CMD10: READ_CID";
	unsigned char tp24[]="\n\rSending CMD9: READ_CSD";
   	unsigned char tp25[]="\n\rCard Type CARDTYPE_SDV2_HC";
	unsigned char tp26[]="\n\rSD_ReadConfiguration() End";

	for(j=0;tp20[j];j++)  	//transmit a predefined string
    	uart_TxChar(tp20[j]);

    retv = SD_FALSE;

    /* Read OCR */
	for(j=0;tp21[j];j++)  	//transmit a predefined string
    	uart_TxChar(tp21[j]);
    if (SD_SendCommand(READ_OCR, 0, CardConfig.ocr, 4) != R1_NO_ERROR) goto end;

/****************Mathew *********************/
	for(j=0;tp22[j];j++)  	//transmit a predefined string
    	uart_TxChar(tp22[j]);
#if 1
		{
			byte = CardConfig.ocr[0];
			hn = byte >> 4;
  			byte = byte << 4;
  			byte = byte >> 4;
  			ln = byte;
			send[0]	= ascii_string[hn]; 
			send[1]	= ascii_string[ln]; 
		   	
			uart_TxChar(send[0]);
			uart_TxChar(send[1]);
		}
		{
			byte = CardConfig.ocr[1];
			hn = byte >> 4;
  			byte = byte << 4;
  			byte = byte >> 4;
  			ln = byte;
			send[0]	= ascii_string[hn]; 
			send[1]	= ascii_string[ln]; 
		   	
			uart_TxChar(send[0]);
			uart_TxChar(send[1]);
		}
		{
			byte = CardConfig.ocr[2];
			hn = byte >> 4;
  			byte = byte << 4;
  			byte = byte >> 4;
  			ln = byte;
			send[0]	= ascii_string[hn]; 
			send[1]	= ascii_string[ln]; 
		   	
			uart_TxChar(send[0]);
			uart_TxChar(send[1]);
		}
		{
			byte = CardConfig.ocr[3];
			hn = byte >> 4;
  			byte = byte << 4;
  			byte = byte >> 4;
  			ln = byte;
			send[0]	= ascii_string[hn]; 
			send[1]	= ascii_string[ln]; 
		   	
			uart_TxChar(send[0]);
			uart_TxChar(send[1]);
		}


#endif
/*******************************************/
  		
    /* Read CID */
	for(j=0;tp23[j];j++)  	//transmit a predefined string
    uart_TxChar(tp23[j]);
    if ((SD_SendCommand(SEND_CID, 0, NULL, 0) != R1_NO_ERROR) ||
        SD_RecvDataBlock (CardConfig.cid, 16)==SD_FALSE) goto end;

    /* Read CSD */
	for(j=0;tp24[j];j++)  	//transmit a predefined string
    uart_TxChar(tp24[j]);
    if ((SD_SendCommand(SEND_CSD, 0, NULL, 0) != R1_NO_ERROR) ||
        SD_RecvDataBlock (CardConfig.csd, 16)==SD_FALSE) goto end;

    /* sector size */
    CardConfig.sectorsize = 512;
    
    /* sector count */
    if (((CardConfig.csd[0]>>6) & 0x3) == 0x1) /* CSD V2.0 (for High/eXtended Capacity) */
    {
        /* Read C_SIZE */
        c_size =  (((uint32_t)CardConfig.csd[7]<<16) + ((uint32_t)CardConfig.csd[8]<<8) + CardConfig.csd[9]) & 0x3FFFFF;
        /* Calculate sector count */
       CardConfig.sectorcnt = (c_size + 1) * 1024;

    } else   /* CSD V1.0 (for Standard Capacity) */
    {
        /* C_SIZE */
        c_size = (((uint32_t)(CardConfig.csd[6]&0x3)<<10) + ((uint32_t)CardConfig.csd[7]<<2) + (CardConfig.csd[8]>>6)) & 0xFFF;
        /* C_SIZE_MUTE */
        c_size_mult = ((CardConfig.csd[9]&0x3)<<1) + ((CardConfig.csd[10]&0x80)>>7);
        /* READ_BL_LEN */
        read_bl_len = CardConfig.csd[5] & 0xF;
        /* sector count = BLOCKNR*BLOCK_LEN/512, we manually set SECTOR_SIZE to 512*/
        //CardConfig.sectorcnt = (c_size+1)*(1<<read_bl_len) * (1<<(c_size_mult+2)) / 512;
        CardConfig.sectorcnt = (c_size+1) << (read_bl_len + c_size_mult - 7);        
    }

    /* Get erase block size in unit of sector */
    switch (CardType)
    {
        case CARDTYPE_SDV2_SC:
        case CARDTYPE_SDV2_HC:
			for(j=0;tp25[j];j++)  	//transmit a predefined string
    			uart_TxChar(tp25[j]);
            if ((SD_SendACommand (SD_STATUS, 0, buf, 1) !=  R1_NO_ERROR) ||
                SD_RecvDataBlock(buf, 16) == SD_FALSE) goto end;      /* Read partial block */    
            for (i=64-16;i;i--) SPI_RecvByte();  /* Purge trailing data */            
            CardConfig.blocksize = 16UL << (buf[10] >> 4); /* Calculate block size based on AU size */
            break;
        case CARDTYPE_MMC:
            //CardConfig.blocksize = ((uint16_t)((CardConfig.csd[10] & 124) >> 2) + 1) * (((CardConfig.csd[11] & 3) << 3) + ((CardConfig.csd[11] & 224) >> 5) + 1);
            CardConfig.blocksize = ((uint16_t)((CardConfig.csd[10] & 124) >> 2) + 1) * (((CardConfig.csd[10] & 3) << 3) + ((CardConfig.csd[11] & 224) >> 5) + 1);
            break;
        case CARDTYPE_SDV1:
            CardConfig.blocksize = (((CardConfig.csd[10] & 63) << 1) + ((uint16_t)(CardConfig.csd[11] & 128) >> 7) + 1) << ((CardConfig.csd[13] >> 6) - 1);
            break;
        default:
            goto end;                
    }

    retv = SD_TRUE;
end:
    SD_DeSelect ();
    for(j=0;tp26[j];j++)  	//transmit a predefined string
    	uart_TxChar(tp26[j]);
    return retv;
}

/**
  * @brief  Receive a data block with specified length from SD/MMC. 
  *
  * @param  buf: Pointer to the data array to store the received data
  * @param  len: Specifies the length (in byte) to be received.
  *              The value should be a multiple of 4.
  * @retval SD_TRUE or SD_FALSE
***/
SD_BOOL SD_RecvDataBlock (uint8_t *buf, uint32_t len)
{
    uint8_t datatoken;
	unsigned int i, Timer1; // Mathew Added
    /* Read data token (0xFE) */
	Timer1 = 10;   /* Data Read Timerout: 100ms */
	do {							
		datatoken = SPI_RecvByte ();
        if (datatoken == 0xFE) break;
	} while (Timer1);
	if(datatoken != 0xFE) return (SD_FALSE);	/* data read timeout */

    /* Read data block */
#ifdef USE_FIFO
    SPI_RecvBlock_FIFO (buf, len);
#else
    for (i = 0; i < len; i++) {
        buf[i] = SPI_RecvByte ();
    }
#endif

    /* 2 bytes CRC will be discarded. */
    SPI_RecvByte ();
    SPI_RecvByte ();

    return (SD_TRUE);
}

/**
  * @brief  Wait for the card is ready. 
  *
  * @param  None
  * @retval SD_TRUE: Card is ready for read commands.
  *         SD_FALSE: Card is not ready 
***/
SD_BOOL SD_WaitForReady (void)
{
    uint32_t Timer = 50000;    // 500ms
    SPI_RecvByte(); /* Read a byte (Force enable DO output) */
    do {
        if (SPI_RecvByte () == 0xFF) return SD_TRUE;
		Timer--;
    } while (Timer);

    return SD_FALSE;
}

/**
  * @brief  Read single or multiple sector(s) from memory card.
  *
  * @param  sect: Specifies the starting sector index to read
  * @param  buf:  Pointer to byte array to store the data
  * @param  cnt:  Specifies the count of sectors to read
  * @retval SD_TRUE or SD_FALSE. 
***/
SD_BOOL SD_ReadSector (uint32_t sect, uint8_t *buf, uint32_t cnt)
{
    SD_BOOL flag;

    /* Convert sector-based address to byte-based address for non SDHC */
    if (CardType != CARDTYPE_SDV2_HC) sect <<= 9;

    flag = SD_FALSE;

    if (cnt > 1) /* Read multiple block */
    {
		if (SD_SendCommand(READ_MULTIPLE_BLOCK, sect, NULL, 0) == R1_NO_ERROR) 
        {            
			do {
				if (SD_RecvDataBlock(buf, SECTOR_SIZE) == SD_FALSE) break;
				buf += SECTOR_SIZE;
			} while (--cnt);

			/* Stop transmission */
            SD_SendCommand(STOP_TRANSMISSION, 0, NULL, 0);				

            /* Wait for the card is ready */
            if (SD_WaitForReady() && cnt==0) flag = SD_TRUE;
        }
    }
    else   /* Read single block */
    {        
        if ((SD_SendCommand(READ_SINGLE_BLOCK, sect, NULL, 0)==R1_NO_ERROR) &&
            SD_RecvDataBlock(buf, SECTOR_SIZE)==SD_TRUE)    
            flag = SD_TRUE;        
    }

    /* De-select the card */
    SD_DeSelect();

    return (flag);
}

/**
  * @brief  Write single or multiple sectors to SD/MMC. 
  *
  * @param  sect: Specifies the starting sector index to write
  * @param  buf: Pointer to the data array to be written
  * @param  cnt: Specifies the number sectors to be written
  * @retval SD_TRUE or SD_FALSE
***/
SD_BOOL SD_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt)
{
    SD_BOOL flag;

    /* Convert sector-based address to byte-based address for non SDHC */
    if (CardType != CARDTYPE_SDV2_HC) sect <<= 9; 

    flag = SD_FALSE;
    if (cnt > 1)  /* write multiple block */
    { 
        if (SD_SendCommand (WRITE_MULTIPLE_BLOCK, sect, NULL, 0) == R1_NO_ERROR)
        {
            do {
                if (SD_SendDataBlock (buf, 0xFC, SECTOR_SIZE) == SD_FALSE)  break;
                buf += SECTOR_SIZE;
            } while (--cnt);

            /* Send Stop Transmission Token. */
            SPI_SendByte (0xFD);
        
            /* Wait for complete */
            if (SD_WaitForReady() && cnt==0) flag = SD_TRUE;
        }
    }
    else  /* write single block */
    {
    
        if ( (SD_SendCommand (WRITE_SINGLE_BLOCK, sect, NULL, 0) == R1_NO_ERROR) &&
            (SD_SendDataBlock (buf, 0xFE, SECTOR_SIZE) == SD_TRUE))
            flag = SD_TRUE;
    }

    /* De-select the card */
    SD_DeSelect();

    return (flag);
}

/**
  * @brief  Send a data block with specified length to SD/MMC. 
  *
  * @param  buf: Pointer to the data array to store the received data
  * @param  tkn: Specifies the token to send before the data block
  * @param  len: Specifies the length (in byte) to send.
  *              The value should be 512 for memory card.
  * @retval SD_TRUE or SD_FALSE
***/
SD_BOOL SD_SendDataBlock (const uint8_t *buf, uint8_t tkn, uint32_t len) 
{
    uint8_t recv;
    int i, Timer1;
    /* Send Start Block Token */
    SPI_SendByte (tkn);

    /* Send data block */
#ifdef USE_FIFO
    SPI_SendBlock_FIFO (buf, len);
#else
    for (i = 0; i < len; i++) 
    {
      SPI_SendByte (buf[i]);
    }
#endif

    /* Send 2 bytes dummy CRC */
    SPI_SendByte (0xFF);
    SPI_SendByte (0xFF);

    /* Read data response to check if the data block has been accepted. */
    if (( (SPI_RecvByte ()) & 0x0F) != 0x05)
        return (SD_FALSE); /* write error */

    /* Wait for wirte complete. */
    Timer1 = 2000000;  // 200ms Mathew  modified
    do {
        recv = SPI_RecvByte();
        if (recv == 0xFF) break; 
		Timer1--; 
    } while (Timer1);

    if (recv == 0xFF) return SD_TRUE;       /* write complete */
    else              return (SD_FALSE);    /* write time out */

}



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
	int i;
	char ret, temp;
	unsigned char tp1[]="\n\r Inside disk_write()";
	unsigned char tp2[]="\n\r Inside SD_WriteSector(): ";
	for(i=0;tp1[i];i++)  //transmit a predefined string
        	uart_TxChar(tp1[i]);
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
//	if (Stat & STA_PROTECT) return RES_WRPRT;

	//if ( SD_WriteSector(sector, buff, count) == SD_TRUE)
	//	return RES_OK;
	ret = SD_WriteSector(sector, buff, count);
	for(i=0;tp2[i];i++)  //transmit a predefined string
        	uart_TxChar(tp2[i]);
	temp = ret;
	hn  = temp >> 4;
  	temp = temp << 4;
  	temp = temp >> 4;
  	ln  = temp;
	send[0]	= ascii_string[hn]; 
	send[1]	= ascii_string[ln];  
	uart_TxChar(send[0]);
	uart_TxChar(send[1]); 

	if(ret == SD_TRUE)
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
   int i; 
   unsigned char tp1[]="\n\r get_fattime()";
   for(i=0;tp1[i];i++)  //transmit a predefined string
        	uart_TxChar(tp1[i]); 
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
	second++;

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
