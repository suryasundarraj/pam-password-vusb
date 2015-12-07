/***********************************************************************************
PAM AUTHENTICATION DEVICE
************************************************************************************
This Module is to Store the Password for the PAM MODULE AND Retrive it 
using the PAMTOOL Command Line Tool
***********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pampwd.h"
#include "usbconfig.h"  /* for device VID, PID, vendor name and product name */

static char *usbErrorMessage(int errCode)
{
static char buffer[80];

    switch(errCode){
        case USBOPEN_ERR_ACCESS:      return "Access to device denied";
        case USBOPEN_ERR_NOTFOUND:    return "The specified device was not found";
        case USBOPEN_ERR_IO:          return "Communication error with device";
        default:
            sprintf(buffer, "Unknown USB error %d", errCode);
            return buffer;
    }
    return NULL;    /* not reached */
}

static usbDevice_t  *openDevice(void)
{
usbDevice_t     *dev = NULL;
unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
char            vendorName[] = {USB_CFG_VENDOR_NAME, 0}, productName[] = {USB_CFG_DEVICE_NAME, 0};
int             vid = rawVid[0] + 256 * rawVid[1];
int             pid = rawPid[0] + 256 * rawPid[1];
int             err;

    if((err = usbhidOpenDevice(&dev, vid, vendorName, pid, productName, 0)) != 0){
        fprintf(stderr, "error finding %s: %s\n", productName, usbErrorMessage(err));
        return NULL;
    }
    return dev;
}

static void hexdump(char *buffer, int len)
{
int     i;
FILE    *fp = stdout;

    for(i = 0; i < 10; i++){
        if(i != 0){
            if(i % 16 == 0){
                fprintf(fp, "\n");
            }else{
                //fprintf(fp, " ");
            }
        }
        fprintf(fp, "%c", buffer[i] & 0xff);
    }
    if(i != 0)
        fprintf(fp, "\n");
}

static void usage(char *myName)
{
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s read\n", myName);
    fprintf(stderr, "  %s write <listofbytes>\n", myName);
}

/**************************************************************************************
Function Name       :   main
Description         :   Read and Write the Password in the EEPROM
Parameters          :   commandline arguments
Return              :   NULL
**************************************************************************************/
int main(int argc, char **argv)
{
usbDevice_t *dev;
char        buffer[129];    /* room for dummy report ID */
int         err;

    if(argc < 2){
        usage(argv[0]);
        exit(1);
    }
    if((dev = openDevice()) == NULL)
        exit(1);
    if(strcasecmp(argv[1], "read") == 0){
        int len = sizeof(buffer);
        if((err = usbhidGetReport(dev, 0, buffer, &len)) != 0){
            fprintf(stderr, "error reading data: %s\n", usbErrorMessage(err));
        }else{
            hexdump(buffer + 1, sizeof(buffer) - 1);
        }
    }else if(strcasecmp(argv[1], "write") == 0){
        memset(buffer, 0, sizeof(buffer));
        int i = 1;
        while(*argv[2] != '\0'){    
            buffer[i] += *argv[2]++;
            i++;
        }
        if((err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0)   /* add a dummy report ID */
            fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));
    }else{
        usage(argv[0]);
        exit(1);
    }
    usbhidCloseDevice(dev);
    return 0;
}

//****************************************************************************************************//