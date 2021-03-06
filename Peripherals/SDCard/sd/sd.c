
#include <stdint.h>
#include "debug.h"
#include "type.h"
#include "sd.h"
#include "spi.h"
#include "uart0.h"
#include "diskio.h"
#include "media_access.h"

extern char ascii_string[];
CARDCONFIG CardConfig;

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
	unsigned int i, Timer1;
    uint8_t  r1, buf[4];
	char tp0[]="\n\rSD_Init() Start";
	char tp1[]="\n\rSending 80 1's";
	char tp2[]="\n\rSending CMD0: GO_IDLE_STATE";
	char tp3[]="\n\rCard enters IDLE state....";
	char tp4[]="\n\rSending CMD8: SEND_IF_COND";
	char tp5[]="\n\rSending ACMD41: SEND_OP_COND";
	char tp6[]="\n\rSending CMD58: READ_OCR";
	char tp7[]="\n\rSending CMD16: SET_BLOCKLEN";
	char tp8[]="\n\rSD_Init() End";

	printString(tp0);
	
   	/* Set card type to unknown */
   	CardType = CARDTYPE_UNKNOWN;

	/* Init SPI interface */
 	SPI_Init ();

	/* Before reset, Send at least 74 clocks at low frequency 
   	(between 100kHz and 400kHz) with CS high and DI (MISO) high. */
    SD_DeSelect();
	
	printString(tp1);
    
	SPI_ConfigClockRate (SPI_CLOCKRATE_LOW);
    for (i = 0; i < 10; i++) SPI_SendByte (0xFF);


   	/* Send CMD0 with CS low to enter SPI mode and reset the card.
   	The card will enter SPI mode if CS is low during the reception of CMD0. 
   	Since the CMD0 (and CMD8) must be sent as a native command, the CRC field
   	must have a valid value. */
		
	printString(tp2);	

	if (SD_SendCommand (GO_IDLE_STATE, 0, NULL, 0) != R1_IN_IDLE_STATE) // CMD0
    {
    	goto  init_end;
    }
	/* Now the card enters IDLE state. */

	printString(tp3);
	printString(tp4);

	/* Card type identification Start ... */
	/* Check the card type, needs around 1000ms */    
    r1 = SD_SendCommand (SEND_IF_COND, 0x1AA, buf, 4);  // CMD8
    if (r1 & 0x80) goto init_end;

	 Timer1 = 1000000; // 1000ms - Mathew added this Hack 
     if (r1 == R1_IN_IDLE_STATE) { /* It's V2.0 or later SD card */
        if (buf[2]!= 0x01 || buf[3]!=0xAA) goto init_end; 

        /* The card is SD V2 and can work at voltage range of 2.7 to 3.6V */
		printString(tp5);
        do {Timer1 --; 
            r1 = SD_SendACommand (SD_SEND_OP_COND, 0x40000000, NULL, 0);  // ACMD41
            if      (r1 == 0x00) break;
            else if (r1 > 0x01)  goto init_end;            
        } while (Timer1);

		printString(tp6);

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
		printString(tp7);
        if (SD_SendCommand (SET_BLOCKLEN, SECTOR_SIZE, NULL, 0) != R1_NO_ERROR)
            CardType = CARDTYPE_UNKNOWN;
    }

init_end:
	printString(tp8);

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
    uint8_t buf[16], reg;
    uint32_t i, c_size, c_size_mult, read_bl_len;
    SD_BOOL retv;

	char tp0[]="\n\n\rSD_ReadConfiguration() Start";
	char tp1[]="\n\rSending CMD58: READ_OCR";
	char tp2[]="\n\rResponse to CMD58: READ_OCR: ";
	char tp3[]="\n\rSending CMD10: READ_CID";
	char tp4[]="\n\rSending CMD9: READ_CSD";
   	char tp5[]="\n\rCard Type CARDTYPE_SDV2_HC";
	char tp6[]="\n\rSD_ReadConfiguration() End";
	
	printString(tp0);

    retv = SD_FALSE;

    /* Read OCR */
	printString(tp1);
    if (SD_SendCommand(READ_OCR, 0, CardConfig.ocr, 4) != R1_NO_ERROR) goto end;

	printString(tp2);
	reg = CardConfig.ocr[0];
	printChar_BCD(reg);
	reg	= CardConfig.ocr[1];
	printChar_BCD(reg);
	reg = CardConfig.ocr[2];
	printChar_BCD(reg);
	reg = CardConfig.ocr[3];
	printChar_BCD(reg);
  		
    /* Read CID */
	printString(tp3);
    if ((SD_SendCommand(SEND_CID, 0, NULL, 0) != R1_NO_ERROR) ||
        SD_RecvDataBlock (CardConfig.cid, 16)==SD_FALSE) goto end;

    /* Read CSD */
	printString(tp4);
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
			printString(tp5);
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
    printString(tp6);
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
	  if (SD_SendCommand (WRITE_MULTIPLE_BLOCK, sect, NULL, 0) == R1_NO_ERROR){
            do {
                if (SD_SendDataBlock (buf, 0xFC, SECTOR_SIZE) == SD_FALSE)  break;
                buf += SECTOR_SIZE;
            } while (--cnt);

            /* Send Stop Transmission Token. */
            SPI_SendByte (0xFD);
        
            /* Wait for complete */
            if (SD_WaitForReady() && cnt==0) flag = SD_TRUE;
        }
    }else{  /* write single block */
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
