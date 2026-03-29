/*=============================================================================
FILE: vx10k_browser_reenable.c
=============================================================================*/

/*=============================================================================
This QUALCOMM Sample Code Disclaimer applies to the sample code of 
QUALCOMM Incorporated ("QUALCOMM") to which it is attached or in which it is 
integrated ("Sample Code").  Qualcomm is a trademark of, and may not be used 
without express written permission of, QUALCOMM.

Disclaimer of Warranty. Unless required by applicable law or agreed to in 
writing, QUALCOMM provides the Sample Code on an "as is" basis, without 
warranties or conditions of any kind, either express or implied, including, 
without limitation, any warranties or conditions of title, non-infringement, 
merchantability, or fitness for a particular purpose. You are solely 
responsible for determining the appropriateness of using the Sample Code and 
assume any risks associated therewith. PLEASE BE ADVISED THAT QUALCOMM WILL NOT
SUPPORT THE SAMPLE CODE OR TROUBLESHOOT ANY ISSUES THAT MAY ARISE WITH IT.

Limitation of Liability.  In no event shall QUALCOMM be liable for any direct, 
indirect, incidental, special, exemplary, or consequential damages (including, 
but not limited to, procurement of substitute goods or services; loss of use, 
data, or profits; or business interruption) however caused and on any theory
of liability, whether in contract, strict liability, or tort (including 
negligence or otherwise) arising in any way out of the use of the Sample Code 
even if advised of the possibility of such damage.
=============================================================================*/

/*-----------------------------------------------------------------------------
Includes and Variable Definitions
-----------------------------------------------------------------------------*/
#include "AEEModGen.h"          // Module interface definitions.
#include "AEEAppGen.h"          // Applet interface definitions.
#include "AEEShell.h"           // Shell interface definitions. 
#include "AEEStdLib.h"

#include "Cvx10k_browser_reenable.h"

typedef struct _vx10k_browser_reenable {
    AEEApplet  applet;    // First element of this structure must be AEEApplet.
    IDisplay * piDisplay; // Copy of IDisplay Interface pointer for easy access.
    IShell   * piShell;   // Copy of IShell Interface pointer for easy access.
    AEEDeviceInfo  deviceInfo; // Copy of device info for easy access.
    // Add your own variables here...

} vx10k_browser_reenable;

/*-----------------------------------------------------------------------------
Function Prototypes
-----------------------------------------------------------------------------*/
static  boolean vx10k_browser_reenable_HandleEvent(vx10k_browser_reenable* pMe, 
                                                AEEEvent eCode,
                                                uint16 wParam, uint32 dwParam);
boolean vx10k_browser_reenable_InitAppData(vx10k_browser_reenable* pMe);
void    vx10k_browser_reenable_FreeAppData(vx10k_browser_reenable* pMe);
static void vx10k_browser_reenable_DrawScreen(vx10k_browser_reenable * pMe);

/*-----------------------------------------------------------------------------
Function Definitions
-----------------------------------------------------------------------------*/

/*=============================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION:
    This function is invoked while the app is being loaded. All modules must 
    provide this function. Ensure to retain the same name and parameters for 
    this function. In here, the module must verify the ClassID and then invoke 
    the AEEApplet_New() function that has been provided in AEEAppGen.c. 

    After invoking AEEApplet_New(), this function can do app-specific 
    initialization. In this example, a generic structure is provided so that 
    app developers need not change the app-specific initialization section
    every time, except for a call to IDisplay_InitAppData().
    
    This is done as follows:
    InitAppData() is called to initialize the AppletData instance. It is the
    app developer's responsibility to fill in the app data initialization code
    of InitAppData(). The app developer is also responsible for releasing
    memory allocated for data contained in AppletData. This is done in 
    IDisplay_FreeAppData().

PROTOTYPE:
    int AEEClsCreateInstance(AEECLSID ClsId,
                             IShell * piShell, 
                             IModule * piModule,
                             void ** ppObj)

PARAMETERS:
    ClsId: [in]:
      Specifies the ClassID of the applet which is being loaded.

    piShell [in]:
      Contains pointer to the IShell object. 

    piModule [in]:
      Contains pointer to the IModule object of the current module to which this
      app belongs.

    ppObj [out]: 
      On return, *ppObj must point to a valid IApplet structure. Allocation of
      memory for this structure and initializing the base data members is done
      by AEEApplet_New().

DEPENDENCIES:
    None

RETURN VALUE:
    AEE_SUCCESS:
      If this app needs to be loaded and if AEEApplet_New()invocation was
      successful.
   
   AEE_EFAILED:
     If this app does not need to be loaded or if errors occurred in
     AEEApplet_New(). If this function returns FALSE, this app will not load.

SIDE EFFECTS:
    None
=============================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell * piShell, IModule * piModule, 
						 void ** ppObj)
{
    *ppObj = NULL;

    // Confirm this applet is the one intended to be created (classID matches):
    if( AEECLSID_CVX10K_BROWSER_REENABLE == ClsId ) {
        // Create the applet and make room for the applet structure.
        // NOTE: FreeAppData is called after EVT_APP_STOP is sent to
        //       HandleEvent.
	    if( TRUE == AEEApplet_New(sizeof(vx10k_browser_reenable),
                        ClsId,
                        piShell,
                        piModule,
                        (IApplet**)ppObj,
                        (AEEHANDLER)vx10k_browser_reenable_HandleEvent,
                        (PFNFREEAPPDATA)vx10k_browser_reenable_FreeAppData) ) {
                     		
            // Initialize applet data. This is called before EVT_APP_START is
            // sent to the HandleEvent function.
		    if(TRUE == vx10k_browser_reenable_InitAppData((vx10k_browser_reenable*)*ppObj)) {
			    return AEE_SUCCESS; // Data initialized successfully.
		    }
		    else {
                // Release the applet. This will free the memory allocated for
                // the applet when AEEApplet_New was called.
                IApplet_Release((IApplet*)*ppObj);
                return AEE_EFAILED;
            }
        } // End AEEApplet_New
    }
    return AEE_EFAILED;
}


/*=============================================================================
FUNCTION: vx10k_browser_reenable_InitAppData

DESCRIPTION:
    This function is called when the application is starting up, so the 
	initialization and resource allocation code is executed here.

PROTOTYPE:
    boolean vx10k_browser_reenable_InitAppData(vx10k_browser_reenable * pMe)

PARAMETERS:
    pMe [in]:
      Pointer to the AEEApplet structure. This structure contains information
      specific to this applet. It was initialized in AEEClsCreateInstance().
  
DEPENDENCIES:
    None

RETURN VALUE:
    TRUE:
      If there were no failures.

SIDE EFFECTS:
    None
=============================================================================*/
boolean vx10k_browser_reenable_InitAppData(vx10k_browser_reenable * pMe)
{
    // Save local copy for easy access:
    pMe->piDisplay = pMe->applet.m_pIDisplay;
    pMe->piShell   = pMe->applet.m_pIShell;

    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->deviceInfo structure.
    // Check the API reference guide for all the handy device info you can get.
    pMe->deviceInfo.wStructSize = sizeof(pMe->deviceInfo);
    ISHELL_GetDeviceInfo(pMe->applet.m_pIShell,&pMe->deviceInfo);
    
    // Insert your code here for initializing or allocating resources...

    return TRUE;// No failures up to this point, so return success.
}


/*=============================================================================
FUNCTION: vx10k_browser_reenable_FreeAppData

DESCRIPTION:
    This function is called to free the allocated resources, etc. when the
    application is exiting.
	

PROTOTYPE:
    void vx10k_browser_reenable_HandleEvent(vx10k_browser_reenable * pMe)

PARAMETERS:
    pMe [in]:
      Pointer to the AEEApplet structure. This structure contains information
      specific to this applet. It was initialized in AEEClsCreateInstance().
  
DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    None
=============================================================================*/
void vx10k_browser_reenable_FreeAppData(vx10k_browser_reenable * pMe)
{
    // Insert your code here for freeing any resources you have allocated...

    // Example to use for releasing each interface:
    // if ( NULL != pMe->piMenuCtl ) {     // check for NULL first
    //     IMenuCtl_Release(pMe->piMenuCtl)// release the interface
    //     pMe->piMenuCtl = NULL;          // set to NULL so no problems later
    // }
}


/*=============================================================================
FUNCTION: vx10k_browser_reenable_HandleEvent

DESCRIPTION:
    This is the EventHandler for this app. All events to this app are handled 
    in this function. All APPs must supply an Event Handler.

PROTOTYPE:
    static boolean vx10k_browser_reenable_HandleEvent(
                                  IApplet * pMe, 
                                  AEEEvent eCode, 
                                  uint16 wParam,
                                  uint32 dwParam)

PARAMETERS:
    pMe [in]:
      Pointer to the AEEApplet structure. This structure contains information
      specific to this applet. It was initialized in AEEClsCreateInstance().
  
    eCode [in]:
      Specifies the Event sent to this applet

    wParam [in],
    dwParam [in]: 
      Event specific data.

DEPENDENCIES:
    None

RETURN VALUE:
    TRUE:
      If the app has processed the event.

    FALSE:
      If the app did not process the event.

SIDE EFFECTS:
    None
=============================================================================*/
static boolean vx10k_browser_reenable_HandleEvent(vx10k_browser_reenable* pMe, 
								AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    switch (eCode) {
        // Event to inform app to start, so start-up code is here:
        case EVT_APP_START:
            // Draw text on display screen.
            vx10k_browser_reenable_DrawScreen(pMe);
            return TRUE;         

        // Event to inform app to exit, so shut-down code is here:
        case EVT_APP_STOP:
      	    return TRUE;

        // Event to inform app to suspend, so suspend code is here:
        case EVT_APP_SUSPEND:
      	    return TRUE;
        case EVT_APP_RESUME:
            vx10k_browser_reenable_DrawScreen(pMe); 
      	    return TRUE;
        case EVT_APP_MESSAGE:
      	    return TRUE;

        // A key was pressed:
        case EVT_KEY:
      	    return FALSE;
        case EVT_FLIP:
            return TRUE;
        case EVT_KEYGUARD:
            return TRUE;

        default:
            break;
    }
    return FALSE; // Event wasn't handled.
}


//Locations of "Network Unavailable, Retry Later"
#define FIRST_STRING 0x00fb5b07
#define SECOND_STRING 0x01a37c08

//"This page has no contents" html page
#define INTERNAL_HTML_PTR 0x016d900c
//Length, including null terminator byte.
#define INTERNAL_HTML_LENGTH 182

//Our replacement string.
//<input type="text" id="a" style="font-size:50pt"/><button onclick="eval(document.getElementById('a').value)">GO</button>
unsigned char replacement_html[]="<input type=\"text\" id=\"a\" style=\"font-size:100pt\"/><button onclick=\"eval(document.getElementById('a').value)\">GO</button>";
#define REPLACEMENT_HTML_LEN 121

static void vx10k_browser_reenable_DrawScreen(vx10k_browser_reenable * pMe)
{
	unsigned char *patch_point;
	patch_point=(unsigned char*)0x0146a34c;
	//$0028->$0128; cmp r0,0 -> cmp r0,1
	patch_point[0]=0x01;
	patch_point[1]=0x28;
	//I can't believe that fucking worked lol. We're in the browser!
	//Unfortonately we can't open any "data:" pages, it still complains, but hey! It opens again!
	//In order to find bugs in the browser's javascript engine, we need to replace the default
	//"This page has no contents" page with something useful.
	patch_point=(unsigned char*)INTERNAL_HTML_PTR;
	for(int i=0;i<INTERNAL_HTML_LENGTH;i++){
		patch_point[i]=0;
	}
	for(int i=0;i<REPLACEMENT_HTML_LEN;i++){
		patch_point[i]=replacement_html[i];
	}

	//That awful "hack" for the AT modem I did in order to get dynamic RAM dumps off the phone.
	//It's outlined here: https://www.tumblr.com/userse31/811007681987051520/live-ram-dumps-on-the-lg-voyager?source=share
	//Set max +ILRR value:
	patch_point=(unsigned char*)0x00f463c0;
	patch_point[0]=0xFF;
	patch_point[1]=0xFF;
	patch_point[2]=0xFF;
	patch_point[3]=0xFF;
	//Set max +EFCS value:
	patch_point=(unsigned char*)0x015b2bcc;
	patch_point[0]=0xFF;
	patch_point[1]=0xFF;
	patch_point[2]=0xFF;
	patch_point[3]=0xFF;
	//Set +ILRR to modify +EFCS pointer:
	patch_point=(unsigned char*)0x001ac8f8;
	patch_point[0]=0xF0;
	patch_point[1]=0xc7;
	patch_point[2]=0x1a;
	patch_point[3]=0x00;

	ISHELL_CloseApplet(pMe->piShell, FALSE);
	//Self close the program.
}
