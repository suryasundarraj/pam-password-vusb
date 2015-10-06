/***********************************************************************************
PAM AUTHENTICATION DEVICE
************************************************************************************
This Module is to Store the Password for the PAM MODULE AND Retrive it 
using the PAMTOOL Command Line Tool
***********************************************************************************/

//Import the Libraries Required by the Program
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */

PROGMEM const char usbHidReportDescriptor[22] = {    /* USB report descriptor */
    0x06, 0x00, 0xff,              // 	USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // 	USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // 	COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x80,                    //   REPORT_COUNT (128)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // 	END_COLLECTION
};

/* Since we define only one feature report, we don't use report-IDs (which
   would be the first byte of the report). The entire report consists of 128
   opaque data bytes and the following variables store the status of the 
   current data transfer */

static uchar    currentAddress;
static uchar    bytesRemaining;

/**************************************************************************************
Function Name 		:	usbFunctionRead()
Description			:	This function is called when the host sends a chunk of data 
						to the device.
Parameters 			:	data,len
			data    :	data to be written to the usb
			len 	:	length of data
Return 				:	uchar - when uart connection fails returns -1 else 0
**************************************************************************************/
uchar   usbFunctionRead(uchar *data, uchar len)
{
    if(len > bytesRemaining)
        len = bytesRemaining;
    eeprom_read_block(data, (uchar *)0 + currentAddress, len);
    currentAddress += len;
    bytesRemaining -= len;
    return len;
}

/**************************************************************************************
Function Name 		:	usbFunctionWrite()
Description			:	This function is called when the host sends a chunk of data 
						to the device.
Parameters 			:	data,len
			data    :	data to be written to the usb
			len 	:	length of data
Return 				:	uchar - when uart connection fails returns -1 else 0
**************************************************************************************/
uchar   usbFunctionWrite(uchar *data, uchar len)
{
    if(bytesRemaining == 0)
        return 1;               /* end of transfer */
    if(len > bytesRemaining)
        len = bytesRemaining;
    eeprom_write_block(data, (uchar *)0 + currentAddress, len);
    currentAddress += len;
    bytesRemaining -= len;
    return bytesRemaining == 0; /* return 1 if this was the last chunk */
}

/**************************************************************************************
Function Name 		:	uartInit
Description			:	Initialize the UART Serial Communication between the 
						Raspberry Pi and the Atmega 8a Microcontroller
Parameters 			:	void
Return 				:	int - when uart connection fails returns -1 else 0
**************************************************************************************/
usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID class request */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* since we have only one report type, we can ignore the report-ID */
            bytesRemaining = 128;
            currentAddress = 0;
            return USB_NO_MSG;  /* use usbFunctionRead() to obtain data */
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
            /* since we have only one report type, we can ignore the report-ID */
            bytesRemaining = 128;
            currentAddress = 0;
            return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
        }
    }else{
        /* ignore vendor type requests, we don't use any */
    }
    return 0;
}

/**************************************************************************************
Function Name 		:	main
Description			:	Initialize the USB and start the interrupt
Parameters 			:	void
Return 				:	int 
**************************************************************************************/
int main(void)
{
	uchar l_delayCount;
    wdt_enable(WDTO_1S);
    odDebugInit();
    DBG1(0x00, 0, 0);       /* debug output: main starts */
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, 
    						   do this while interrupts are disabled! */
    l_delayCount = 0;
    while(--l_delayCount)
    {             
    	/* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    sei();
    DBG1(0x01, 0, 0);       
    for(;;)
    {                
        DBG1(0x02, 0, 0);   /*debug output: main loop iterates*/
        wdt_reset();
        usbPoll();
    }
    return 0;
}

//End of the Program
