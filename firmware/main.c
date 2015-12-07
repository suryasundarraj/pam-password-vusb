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

static uchar    g_currentAddress;
static uchar    g_bytesRemaining;

/**************************************************************************************
Function Name 		:	usbFunctionRead()
Description			:	This function is called when the host sends a chunk of data 
						to the device.
Parameters 			:	data,len
			p_data  :	data to be written to the usb
			p_len 	:	length of data
Return 				:	uchar - when uart connection fails returns -1 else 0
**************************************************************************************/
uchar   usbFunctionRead(uchar *p_data, uchar p_len)
{
    if(p_len > g_bytesRemaining)
        p_len = g_bytesRemaining;
    eeprom_read_block(p_data, (uchar *)0 + g_currentAddress, p_len);
    g_currentAddress += p_len;
    g_bytesRemaining -= p_len;
    return p_len;
}

/**************************************************************************************
Function Name 		:	usbFunctionWrite()
Description			:	This function is called when the host sends a chunk of data 
						to the device.
Parameters 			:	data,len
			p_data  :	data to be written to the usb
			p_len 	:	length of data
Return 				:	uchar - when uart connection fails returns -1 else 0
**************************************************************************************/
uchar   usbFunctionWrite(uchar *p_data, uchar p_len)
{
    if(g_bytesRemaining == 0)
        return 1;               /* end of transfer */
    if(p_len > g_bytesRemaining)
        p_len = g_bytesRemaining;
    eeprom_write_block(p_data, (uchar *)0 + g_currentAddress, p_len);
    g_currentAddress += p_len;
    g_bytesRemaining -= p_len;
    return g_bytesRemaining == 0; /* return 1 if this was the last chunk */
}

/**************************************************************************************
Function Name 		:	usbFunctionSetup
Description			:	Setup the USB Communication
Parameters 			:	data
Return 				:	error 
**************************************************************************************/
usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID class request */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* since we have only one report type, we can ignore the report-ID */
            g_bytesRemaining = 128;
            g_currentAddress = 0;
            return USB_NO_MSG;  /* use usbFunctionRead() to obtain data */
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
            /* since we have only one report type, we can ignore the report-ID */
            g_bytesRemaining = 128;
            g_currentAddress = 0;
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
Return 				:	NULL
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
//********************************************************************************************//