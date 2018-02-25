
#include <stdint.h>
#include "type.h"
#include "sd.h"
#include "spi.h"
#include "uart0.h"

/* Initilize an array with ASCII values of 0 to F */
// 0 = 48
// ...
// 9 = 57
// A = 65
// ...
// 70 = F  
char ascii_string[16] ={48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70};
unsigned char hn, ln, temp;

CARDCONFIG CardConfig;


/**
  * @brief  Initializes the memory card.
  *
  * @param  None
  * @retval SD_TRUE: Init OK.
  *         SD_FALSE: Init failed. 
  *
  * Note: Refer to the init flow at http://elm-chan.org/docs/mmc/mmc_e.html
  */


SD_BOOL SD_Init (void)
{
	char receive, crdTyp, send[20];	
	unsigned int i, j, Timer1;
    uint8_t  r1, buf[4], my_buf[4],SDCommand_ret;
	unsigned char tp4[]="\n\rResponse to CMD0: ";
	unsigned char tp5[]="\n\rNow the card enters IDLE state....";
	unsigned char tp6[]="\n\rSending SEND_IF_COND Command()";
	unsigned char tp7[]="\n\rResponse to CMD8 (0x1AA): ";
	unsigned char tp8[]="The card is SD V2 and can work at voltage range of 2.7 to 3.6V"; 
	unsigned char tp9[]="\n\rSetting SECTOR SIZE: ";
	unsigned char tp10[]="\n\rInitial setting of CardType: ";
	unsigned char tp11[]="\n\rLater setting of CardType: ";
	unsigned char tp100[]="\n\rInit END ";
    	/* Set card type to unknown */
   	CardType = CARDTYPE_UNKNOWN;
/****************Mathew *********************/
#if 1
		for(j=0;tp10[j];j++)  //transmit a predefined string
    	uart_TxChar(tp10[j]);
		{
			crdTyp = CardType;
			hn = crdTyp >> 4;
  			crdTyp = crdTyp << 4;
  			crdTyp = crdTyp >> 4;
  			ln = crdTyp;
			send[0]	= ascii_string[hn]; 
			send[1]	= ascii_string[ln]; 
		}	
	   	uart_TxChar(send[0]);
		uart_TxChar(send[1]);
#endif
/*******************************************/

	/* Init SPI interface */
 	SPI_Init ();

	/* Before reset, Send at least 74 clocks at low frequency 
   	(between 100kHz and 400kHz) with CS high and DI (MISO) high. */
    	SD_DeSelect();
    	SPI_ConfigClockRate (SPI_CLOCKRATE_LOW);
    	for (i = 0; i < 10; i++){
			receive = SPI_SendByte (0xFF);
			{
				temp = receive;
				hn = temp >> 4;
  				temp = temp << 4;
  				temp = temp >> 4;
  				ln = temp;
				send[i*2] 		= ascii_string[hn]; 
				send[(i*2)+1] 	= ascii_string[ln]; 
			}	
		}
		for(j=0;j<20;j++)  //transmit RECEIVED BYTES
        	uart_TxChar(send[j]);

    	/* Send CMD0 with CS low to enter SPI mode and reset the card.
    	The card will enter SPI mode if CS is low during the reception of CMD0. 
    	Since the CMD0 (and CMD8) must be sent as a native command, the CRC field
    	must have a valid value. */
		
		
    	//if (SD_SendCommand (GO_IDLE_STATE, 0, NULL, 0) != R1_IN_IDLE_STATE) // CMD0
    	//{
        //	goto  init_end;
    	//}
		SDCommand_ret = SD_SendCommand (GO_IDLE_STATE, 0, NULL, 0);
		if (SDCommand_ret != R1_IN_IDLE_STATE) // CMD0		
		{
        	goto  init_end;
    	}

/******* Mathew Added for Debug ************/
#if 1
	for(j=0;tp4[j];j++)  //transmit a predefined string
    	uart_TxChar(tp4[j]);
	{
		hn = SDCommand_ret >> 4;
  		SDCommand_ret = SDCommand_ret << 4;
  		SDCommand_ret = SDCommand_ret >> 4;
  		ln = SDCommand_ret;
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln]; 
	}	
	   	uart_TxChar(send[0]);
		uart_TxChar(send[1]);
#endif
/******************************************/

	for(j=0;tp5[j];j++)  //transmit a predefined string
    	uart_TxChar(tp5[j]);
    	/* Now the card enters IDLE state. */

		
	for(j=0;tp6[j];j++)  //transmit a predefined string
    	uart_TxChar(tp6[j]);
		/* Card type identification Start ... */
		/* Check the card type, needs around 1000ms */    
    r1 = SD_SendCommand (SEND_IF_COND, 0x1AA, buf, 4);  // CMD8
/******* Mathew Added for Debug ************/
 #if 1
	for(j=0;tp7[j];j++)  //transmit a predefined string
    	uart_TxChar(tp7[j]);
    receive = r1;
   	{
		hn = SDCommand_ret >> 4;
  		receive = receive << 4;
  		receive = receive >> 4;
  		ln = receive;
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln]; 
	
	   	uart_TxChar(send[0]);
		uart_TxChar(send[1]);
	}

	my_buf[0]= buf[0];
	my_buf[1]= buf[1];
	my_buf[2]= buf[2];
	my_buf[3]= buf[3];
	// my_buf[0]
	{
		hn = my_buf[0] >> 4;
  		my_buf[0] = my_buf[0] << 4;
  		my_buf[0] = my_buf[0] >> 4;
  		ln = my_buf[0];
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln]; 
		
	   	uart_TxChar(send[0]);
		uart_TxChar(send[1]);
	}
	// my_buf[1]
	{
		hn = my_buf[1] >> 4;
  		my_buf[1] = my_buf[1] << 4;
  		my_buf[1] = my_buf[1] >> 4;
  		ln = my_buf[1];
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln]; 
		
	   	uart_TxChar(send[0]);
		uart_TxChar(send[1]);
	}
	// my_buf[2]
	{
		hn = my_buf[2] >> 4;
  		my_buf[2] = my_buf[2] << 4;
  		my_buf[2] = my_buf[2] >> 4;
  		ln = my_buf[2];
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln]; 
		
	   	uart_TxChar(send[0]);
		uart_TxChar(send[1]);
	}
	// my_buf[3]
	{
		hn = my_buf[3] >> 4;
  		my_buf[3] = my_buf[3] << 4;
  		my_buf[3] = my_buf[3] >> 4;
  		ln = my_buf[3];
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln]; 
		
	   	uart_TxChar(send[0]);
		uart_TxChar(send[1]);
	}

/* now the 4 Buffer Bytes */



#endif
/******************************************/
     if (r1 & 0x80) goto init_end;
#if 1
	 Timer1 = 1000000; // 1000ms
     if (r1 == R1_IN_IDLE_STATE) { /* It's V2.0 or later SD card */
        if (buf[2]!= 0x01 || buf[3]!=0xAA) goto init_end; 

        /* The card is SD V2 and can work at voltage range of 2.7 to 3.6V */
		for(j=0;tp8[j];j++)  //transmit a predefined string
    	uart_TxChar(tp8[j]);
        do {Timer1 --; 
            r1 = SD_SendACommand (SD_SEND_OP_COND, 0x40000000, NULL, 0);  // ACMD41
            if      (r1 == 0x00) break;
            else if (r1 > 0x01)  goto init_end;            
        } while (Timer1);

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

/****************Mathew *********************/
#if 1
		for(j=0;tp11[j];j++)  //transmit a predefined string
    	uart_TxChar(tp11[j]);
		{
			crdTyp = CardType;
			hn = crdTyp >> 4;
  			crdTyp = crdTyp << 4;
  			crdTyp = crdTyp >> 4;
  			ln = crdTyp;
			send[0]	= ascii_string[hn]; 
			send[1]	= ascii_string[ln]; 
		}	
	   	uart_TxChar(send[0]);
		uart_TxChar(send[1]);
#endif
/*******************************************/
		for(j=0;tp9[j];j++)  //transmit a predefined string
    	uart_TxChar(tp9[j]);
	/* For SDHC or SDXC, block length is fixed to 512 bytes, for others,
    the block length is set to 512 manually. */
    if (CardType == CARDTYPE_MMC ||
        CardType == CARDTYPE_SDV1 ||
        CardType == CARDTYPE_SDV2_SC )
    {
        if (SD_SendCommand (SET_BLOCKLEN, SECTOR_SIZE, NULL, 0) != R1_NO_ERROR)
            CardType = CARDTYPE_UNKNOWN;
    }
#endif
init_end:
/**************** Mathew **************/
	for(j=0;tp100[j];j++)  //transmit a predefined string
    	uart_TxChar(tp100[j]);
/**************************************/

   	SD_DeSelect();

    	if (CardType == CARDTYPE_UNKNOWN){
        	return (SD_FALSE);
    	}
    	else{     /* Init OK. use high speed during data transaction stage. */
        	SPI_ConfigClockRate (SPI_CLOCKRATE_HIGH);
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
  */
uint8_t SD_SendCommand (uint8_t cmd, uint32_t arg, uint8_t *buf, uint32_t len)
{
    uint32_t r1,i,j;
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

   	for(j=0;tp4[j];j++)  //transmit a predefined string
        	uart_TxChar(tp4[j]);
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
  */
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
  */
SD_BOOL SD_ReadConfiguration ()
{
    uint8_t buf[16];
    uint32_t i, c_size, c_size_mult, read_bl_len;
    SD_BOOL retv;
  
    retv = SD_FALSE;

    /* Read OCR */
    if (SD_SendCommand(READ_OCR, 0, CardConfig.ocr, 4) != R1_NO_ERROR) goto end;

    /* Read CID */
    if ((SD_SendCommand(SEND_CID, 0, NULL, 0) != R1_NO_ERROR) ||
        SD_RecvDataBlock (CardConfig.cid, 16)==SD_FALSE) goto end;

    /* Read CSD */
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

    return retv;
}

/**
  * @brief  Receive a data block with specified length from SD/MMC. 
  *
  * @param  buf: Pointer to the data array to store the received data
  * @param  len: Specifies the length (in byte) to be received.
  *              The value should be a multiple of 4.
  * @retval SD_TRUE or SD_FALSE
  */
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
  */
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
