// TwainScan.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"
//
//#include "TwainScan.h"
//
//#include <stdexcept>
//
//using namespace std;
//
//
//double Add_hokus(double a, double b)
//{
//    return a + b;
//}
//
//double Add_hokus2(double a, double b)
//{
//    return ((a + b) * 2);
//}


/***************************************************************************
* Copyright © 2007 TWAIN Working Group:  
*   Adobe Systems Incorporated, AnyDoc Software Inc., Eastman Kodak Company, 
*   Fujitsu Computer Products of America, JFL Peripheral Solutions Inc., 
*   Ricoh Corporation, and Xerox Corporation.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the TWAIN Working Group nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY TWAIN Working Group ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL TWAIN Working Group BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************/

/**
* @file main.cpp
* The entry point to launch the application
* @author TWAIN Working Group
* @date October 2007
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TwainScan.h"
#include "TwainCout.h"

#include <stdexcept>

// I found that compiling using the sunfreeware.com stuff on Solaris 9
// required this typedef. This is related to the inclusion of signal.h
#if defined (__SVR4) && defined (__sun)
typedef union {
  long double  _q;
  uint32_t     _l[4];
} upad128_t;
#endif

#include <signal.h>

#include "CommonTWAIN.h"
#include "TwainAppCMD.h"
#include "TwainApp_ui.h"
#include "TwainCout.h"
#include "TwainApp.h"

#include <iostream>
#include <fstream>

using namespace std;

//// just a dummy class to overload tw_cout and tw_cerr
//class tw_ostream{};
//tw_ostream tw_cout;
//
//tw_ostream &operator<< (tw_ostream &ostr, char * a)
//{
//  ofstream myfile;
//  myfile.open ("log.txt", fstream::app);
//  myfile << a;
//  myfile.close();
//
//  return ostr;
//}



//////////////////////////////////////////////////////////////////////////////
// Global Variables
TwainAppCMD  *gpTwainApplicationCMD;  /**< The main application */
extern bool   gUSE_CALLBACKS;         // defined in TwainApp.cpp


double Add_hokus(double a, double b)
{
  return a + b;
}

double Add_hokus2(double a, double b)
{
  return ((a + b) * 2);
}


//////////////////////////////////////////////////////////////////////////////
/** 
* Display exit message.
* @param[in] _sig not used.
*/
void onSigINT(int _sig)
{
  //UNUSEDARG(_sig);
  tw_cout << "\nGoodbye!" << tw_endl;
  exit(0);
}

//////////////////////////////////////////////////////////////////////////////
/** 
* Negotiate a capabilities between the app and the DS
* @param[in] _pCap the capabilities to negotiate
*/
void negotiate_CAP(const pTW_CAPABILITY _pCap)
{
  string input;

  // -Setting one cap could change another cap so always refresh the caps
  // before working with another one.  
  // -Another method of doing this is to let the DS worry about the state
  // of the caps instead of keeping a copy in the app like I'm doing.
  gpTwainApplicationCMD->initCaps();

  for (;;)
  {
    if((TWON_ENUMERATION == _pCap->ConType) || 
       (TWON_ONEVALUE == _pCap->ConType) ||
       (TWON_RANGE == _pCap->ConType))
    {
      TW_MEMREF pVal = _DSM_LockMemory(_pCap->hContainer);

      // print the caps current value
      if(TWON_ENUMERATION == _pCap->ConType) //TWON_ENUMERATION
      {
        print_ICAP(_pCap->Cap, (pTW_ENUMERATION)(pVal));
      }
      else if(TWON_RANGE == _pCap->ConType) //TWON_RANGE
      {
        print_ICAP(_pCap->Cap, (pTW_RANGE)(pVal));
      }
      else // TWON_ONEVALUE
      {
        print_ICAP(_pCap->Cap, (pTW_ONEVALUE)(pVal));
      }

      tw_cout << "\nset cap# > ";
      cin >> input;
      tw_cout << tw_endl;

      if("q" == input)
      {
        _DSM_UnlockMemory(_pCap->hContainer);
        break;
      }
      else
      {
        int n = atoi(input.c_str());
        TW_UINT16  valUInt16 = 0;
		    pTW_FIX32  pValFix32 = {0};
		    pTW_FRAME  pValFrame = {0};

        // print the caps current value
        if(TWON_ENUMERATION == _pCap->ConType)
        {
          switch(((pTW_ENUMERATION)pVal)->ItemType)
          {
            case TWTY_UINT16:
              valUInt16 = ((pTW_UINT16)(&((pTW_ENUMERATION)pVal)->ItemList))[n];
            break;

            case TWTY_FIX32:
              pValFix32 = &((pTW_ENUMERATION_FIX32)pVal)->ItemList[n];
            break;
            
            case TWTY_FRAME:
              pValFrame = &((pTW_ENUMERATION_FRAME)pVal)->ItemList[n];
            break;
          }

          switch(_pCap->Cap)
          {
            case ICAP_PIXELTYPE:
              gpTwainApplicationCMD->set_ICAP_PIXELTYPE(valUInt16);
            break;

            case ICAP_BITDEPTH:
              gpTwainApplicationCMD->set_ICAP_BITDEPTH(valUInt16);
            break;

            case ICAP_UNITS:
              gpTwainApplicationCMD->set_ICAP_UNITS(valUInt16);
            break;
            
            case ICAP_XFERMECH:
              gpTwainApplicationCMD->set_ICAP_XFERMECH(valUInt16);
            break;
          
            case ICAP_XRESOLUTION:
            case ICAP_YRESOLUTION:
              gpTwainApplicationCMD->set_ICAP_RESOLUTION(pValFix32);
            break;

            case ICAP_FRAMES:
              gpTwainApplicationCMD->set_ICAP_FRAMES(pValFrame);
            break;

            default:
              tw_cerr << "Unsupported capability" << tw_endl;
            break;
          }
        }
      }
      _DSM_UnlockMemory(_pCap->hContainer);
    }
    else
    {
      tw_cerr << "Unsupported capability" << tw_endl;
      break;
    }
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
/**
* drives main capabilities menu.  Negotiate all capabilities
*/
void negotiateCaps()
{
  // If the app is not in state 4, don't even bother showing this menu.
  if(gpTwainApplicationCMD->m_DSMState < 4)
  {
    tw_cerr << "\nNeed to open a source first\n" << tw_endl;
    return;
  }

  string input;

  // Loop forever until either SIGINT is heard or user types done to go back
  // to the main menu.
  for (;;)
  {
    printMainCaps();
    tw_cout << "\n(h for help) > ";
    cin >> input;
    tw_cout << tw_endl;

    if("q" == input)
    {
      break;
    }
    else if("h" == input)
    {
      printMainCaps();
    }
    else if("1" == input)
    {
      negotiate_CAP(&(gpTwainApplicationCMD->m_ICAP_XFERMECH));
    }
    else if("2" == input)
    {
      negotiate_CAP(&(gpTwainApplicationCMD->m_ICAP_PIXELTYPE));
    }
    else if("3" == input)
    {
      negotiate_CAP(&(gpTwainApplicationCMD->m_ICAP_BITDEPTH));
    }
    else if("4" == input)
    {
      negotiate_CAP(&(gpTwainApplicationCMD->m_ICAP_XRESOLUTION));
    }
    else if("5" == input)
    {
      negotiate_CAP(&(gpTwainApplicationCMD->m_ICAP_YRESOLUTION));
    }
    else if("6" == input)
    {
      negotiate_CAP(&(gpTwainApplicationCMD->m_ICAP_FRAMES));
    }
    else if("7" == input)
    {
      negotiate_CAP(&(gpTwainApplicationCMD->m_ICAP_UNITS));
    }
    else
    {
      printMainCaps();
    }
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
/**
* Enables the source. The source will let us know when it is ready to scan by
* calling our registered callback function.
*/
void EnableDS_Short(bool gui_enable)
{
  gpTwainApplicationCMD->m_DSMessage = 0;

  // -Enable the data source. This puts us in state 5 which means that we
  // have to wait for the data source to tell us to move to state 6 and
  // start the transfer.  Once in state 5, no more set ops can be done on the
  // caps, only get ops.
  // -The scan will not start until the source calls the callback function
  // that was registered earlier.

  if(!gpTwainApplicationCMD->enableDS(GetDesktopWindow(), gui_enable))
  {
    return;
  }

  // now we have to wait until we hear something back from the DS.
  while(!gpTwainApplicationCMD->m_DSMessage)
  {
    TW_EVENT twEvent = {0};

    // If we are using callbacks, there is nothing to do here except sleep
    // and wait for our callback from the DS.  If we are not using them, 
    // then we have to poll the DSM.

    // Pumping messages is for Windows only
	  MSG Msg;
	  if(!GetMessage((LPMSG)&Msg, NULL, 0, 0))
    {
      break;//WM_QUIT
    }
    twEvent.pEvent = (TW_MEMREF)&Msg;

    twEvent.TWMessage = MSG_NULL;
    TW_UINT16  twRC = TWRC_NOTDSEVENT;
    twRC = _DSM_Entry( gpTwainApplicationCMD->getAppIdentity(),
                gpTwainApplicationCMD->getDataSource(),
                DG_CONTROL,
                DAT_EVENT,
                MSG_PROCESSEVENT,
                (TW_MEMREF)&twEvent);

    if(!gUSE_CALLBACKS && twRC==TWRC_DSEVENT)
    {
      // check for message from Source
      switch (twEvent.TWMessage)
      {
        case MSG_XFERREADY:
        case MSG_CLOSEDSREQ:
        case MSG_CLOSEDSOK:
        case MSG_NULL:
          gpTwainApplicationCMD->m_DSMessage = twEvent.TWMessage;
          break;

        default:
          tw_cerr << "\nError - Unknown message in MSG_PROCESSEVENT loop\n" << tw_endl;
          break;
      }
    }
    if(twRC!=TWRC_DSEVENT)
    {   
      TranslateMessage ((LPMSG)&Msg);
      DispatchMessage ((LPMSG)&Msg);
    }
  }

  // At this point the source has sent us a callback saying that it is ready to
  // transfer the image.

  if(gpTwainApplicationCMD->m_DSMessage == MSG_XFERREADY)
  {
    // move to state 6 as a result of the data source. We can start a scan now.
    gpTwainApplicationCMD->m_DSMState = 6;

    gpTwainApplicationCMD->startScan();
  }

  // Scan is done, disable the ds, thus moving us back to state 4 where we
  // can negotiate caps again.
  gpTwainApplicationCMD->disableDS();

  return;
}

//////////////////////////////////////////////////////////////////////////////
/**
* Enables the source. The source will let us know when it is ready to scan by
* calling our registered callback function.
*/
void EnableDS()
{
  gpTwainApplicationCMD->m_DSMessage = 0;
  #ifdef TWNDS_OS_LINUX

    int test;
    sem_getvalue(&(gpTwainApplicationCMD->m_TwainEvent), &test);
    while(test<0)
    {
      sem_post(&(gpTwainApplicationCMD->m_TwainEvent));    // Event semaphore Handle
      sem_getvalue(&(gpTwainApplicationCMD->m_TwainEvent), &test);
    }
    while(test>0)
    {
      sem_wait(&(gpTwainApplicationCMD->m_TwainEvent)); // event semaphore handle
      sem_getvalue(&(gpTwainApplicationCMD->m_TwainEvent), &test);
    }

  #endif
  // -Enable the data source. This puts us in state 5 which means that we
  // have to wait for the data source to tell us to move to state 6 and
  // start the transfer.  Once in state 5, no more set ops can be done on the
  // caps, only get ops.
  // -The scan will not start until the source calls the callback function
  // that was registered earlier.
#ifdef TWNDS_OS_WIN
  if(!gpTwainApplicationCMD->enableDS(GetDesktopWindow(), TRUE))
#else
  if(!gpTwainApplicationCMD->enableDS(0, TRUE))
#endif
  {
    return;
  }

#ifdef TWNDS_OS_WIN
  // now we have to wait until we hear something back from the DS.
  while(!gpTwainApplicationCMD->m_DSMessage)
  {
    TW_EVENT twEvent = {0};

    // If we are using callbacks, there is nothing to do here except sleep
    // and wait for our callback from the DS.  If we are not using them, 
    // then we have to poll the DSM.

    // Pumping messages is for Windows only
	  MSG Msg;
	  if(!GetMessage((LPMSG)&Msg, NULL, 0, 0))
    {
      break;//WM_QUIT
    }
    twEvent.pEvent = (TW_MEMREF)&Msg;

    twEvent.TWMessage = MSG_NULL;
    TW_UINT16  twRC = TWRC_NOTDSEVENT;
    twRC = _DSM_Entry( gpTwainApplicationCMD->getAppIdentity(),
                gpTwainApplicationCMD->getDataSource(),
                DG_CONTROL,
                DAT_EVENT,
                MSG_PROCESSEVENT,
                (TW_MEMREF)&twEvent);

    if(!gUSE_CALLBACKS && twRC==TWRC_DSEVENT)
    {
      // check for message from Source
      switch (twEvent.TWMessage)
      {
        case MSG_XFERREADY:
        case MSG_CLOSEDSREQ:
        case MSG_CLOSEDSOK:
        case MSG_NULL:
          gpTwainApplicationCMD->m_DSMessage = twEvent.TWMessage;
          break;

        default:
          tw_cerr << "\nError - Unknown message in MSG_PROCESSEVENT loop\n" << tw_endl;
          break;
      }
    }
    if(twRC!=TWRC_DSEVENT)
    {   
      TranslateMessage ((LPMSG)&Msg);
      DispatchMessage ((LPMSG)&Msg);
    }
  }
#elif defined(TWNDS_OS_LINUX)
  // Wait for the event be signaled
  sem_wait(&(gpTwainApplicationCMD->m_TwainEvent)); // event semaphore handle
                            // Indefinite wait
#endif

  // At this point the source has sent us a callback saying that it is ready to
  // transfer the image.

  if(gpTwainApplicationCMD->m_DSMessage == MSG_XFERREADY)
  {
    // move to state 6 as a result of the data source. We can start a scan now.
    gpTwainApplicationCMD->m_DSMState = 6;

    gpTwainApplicationCMD->startScan();
  }

  // Scan is done, disable the ds, thus moving us back to state 4 where we
  // can negotiate caps again.
  gpTwainApplicationCMD->disableDS();

  return;
}

////////////////////////////////////////////////////////////////////////////
/**
* Callback funtion for DS.  This is a callback function that will be called by
* the source when it is ready for the application to start a scan. This 
* callback needs to be registered with the DSM before it can be called.
* It is important that the application returns right away after recieving this
* message.  Set a flag and return.  Do not process the callback in this function.
*/
#ifdef TWH_CMP_MSC
TW_UINT16 FAR PASCAL
#else
FAR PASCAL TW_UINT16 
#endif
DSMCallback(pTW_IDENTITY _pOrigin,
            pTW_IDENTITY _pDest,
            TW_UINT32    _DG,
            TW_UINT16    _DAT,
            TW_UINT16    _MSG,
            TW_MEMREF    _pData)
{ //return TWRC_SUCCESS; }
  //UNUSEDARG(_pDest);
  //UNUSEDARG(_DG);
  //UNUSEDARG(_DAT);
  //UNUSEDARG(_pData);

  TW_UINT16 twrc = TWRC_SUCCESS;

  // we are only waiting for callbacks from our datasource, so validate
  // that the originator.
  if(0 == _pOrigin ||
     _pOrigin->Id != gpTwainApplicationCMD->getDataSource()->Id)
  {
    return TWRC_FAILURE;
  }
  switch (_MSG)
  {
    case MSG_XFERREADY:
    case MSG_CLOSEDSREQ:
    case MSG_CLOSEDSOK:
    case MSG_NULL:
      gpTwainApplicationCMD->m_DSMessage = _MSG;
      // now signal the event semaphore
    #ifdef TWNDS_OS_LINUX
      {
      int test=12345;
      sem_post(&(gpTwainApplicationCMD->m_TwainEvent));    // Event semaphore Handle
  }
    #endif
      break;

    default:
      tw_cerr << "Error - Unknown message in callback routine" << tw_endl;
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}


//////////////////////////////////////////////////////////////////////////////
/**
* main program loop
*/
#ifdef TWH_CMP_MSC
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char *argv[])
#endif
{
  //UNUSEDARG(argc);
  //UNUSEDARG(argv);
  int ret = EXIT_SUCCESS;

  // Instantiate the TWAIN application CMD class
  HWND parentWindow = NULL;

#ifdef TWH_CMP_MSC
  parentWindow = GetConsoleWindow();
#endif
  gpTwainApplicationCMD = new TwainAppCMD(parentWindow);

  // setup a signal handler for SIGINT that will allow the program to stop
  signal(SIGINT, &onSigINT);

  string input;

  printOptions();

  // start the main event loop
  for (;;)
  {
    tw_cout << "\n(h for help) > ";
    cin >> input;
    tw_cout << tw_endl;

    if("q" == input)
    {
      break;
    }
    else if("h" == input)
    {
      printOptions();
    }
    else if("cdsm" == input)
    {
      gpTwainApplicationCMD->connectDSM();
    }
    else if("xdsm" == input)
    {
      gpTwainApplicationCMD->disconnectDSM();
    }
    else if("lds" == input)
    {
      gpTwainApplicationCMD->printAvailableDataSources();
    }
    else if("pds" == input.substr(0,3))
    {
      gpTwainApplicationCMD->printIdentityStruct(atoi(input.substr(3,input.length()-3).c_str()));
    }
    else if("cds" == input.substr(0,3))
    {
      gpTwainApplicationCMD->loadDS(atoi(input.substr(3,input.length()-3).c_str()));
    }
    else if("xds" == input)
    {
      gpTwainApplicationCMD->unloadDS();
    }
    else if("caps" == input)
    {
      if(gpTwainApplicationCMD->m_DSMState < 3)
      {
        tw_cout << "\nYou need to select a source first!" << tw_endl;
      }
      else
      {
        negotiateCaps();
        printOptions();
      }
    }
    else if("scan" == input)
    {
      EnableDS();
    }
    else
    {
      // default action
      printOptions();
    }
  }

  gpTwainApplicationCMD->exit();
  delete gpTwainApplicationCMD;
  gpTwainApplicationCMD = 0;

  return ret;
}


int ts_select(int *id)
{
  ts_initialize();

  tw_ostream tw_cout;
  tw_cout << "Start Selection";
  gpTwainApplicationCMD = new TwainAppCMD(NULL);
  
  gpTwainApplicationCMD->connectDSM();

  gpTwainApplicationCMD->userselectDataSource((TW_UINT32 *)id);

  gpTwainApplicationCMD->disconnectDSM();

  gpTwainApplicationCMD->exit();
  delete gpTwainApplicationCMD;
  gpTwainApplicationCMD = 0;

  return 0;
}


int ts_connection_open(int id)
{
  tw_cout << "----- Open connection! -----" << tw_endl;

  int ret = EXIT_SUCCESS;

  // Instantiate the TWAIN application CMD class
  HWND parentWindow = NULL;

  parentWindow = GetConsoleWindow();

  gpTwainApplicationCMD = new TwainAppCMD(NULL /*parentWindow*/);

  // setup a signal handler for SIGINT that will allow the program to stop
  signal(SIGINT, &onSigINT);

  gpTwainApplicationCMD->connectDSM();

  gpTwainApplicationCMD->loadDS((TW_INT32) id);

  return ret;
}

int ts_connection_close()
{
  tw_cout << "----- Close connection! -----" << tw_endl;

  gpTwainApplicationCMD->unloadDS();

  gpTwainApplicationCMD->disconnectDSM();

  gpTwainApplicationCMD->exit();
  delete gpTwainApplicationCMD;
  gpTwainApplicationCMD = 0;

  return 0;
}


int ts_action (int id, ENUM_TS_ACTION ts_action)
{
	 //UNUSEDARG(argc);
  //UNUSEDARG(argv);
  int ret = EXIT_SUCCESS;

  // Instantiate the TWAIN application CMD class
  HWND parentWindow = NULL;

#ifdef TWH_CMP_MSC
  parentWindow = GetConsoleWindow();
#endif
  gpTwainApplicationCMD = new TwainAppCMD(NULL /*parentWindow*/);

  // setup a signal handler for SIGINT that will allow the program to stop
  signal(SIGINT, &onSigINT);

  gpTwainApplicationCMD->connectDSM();

  gpTwainApplicationCMD->loadDS((TW_INT32) id);

  try {
    switch (ts_action)
    {
      case TS_SCAN:
        EnableDS_Short(false);
        //EnableDS();
      break;

      case TS_GET_CONFIG:
          print_config();
      break;
      
      default:
      break;
    }
  }
  catch(int e)
  {
    tw_cout << "error" << e << '\n';
  }

  gpTwainApplicationCMD->unloadDS();

  gpTwainApplicationCMD->disconnectDSM();

  gpTwainApplicationCMD->exit();
  delete gpTwainApplicationCMD;
  gpTwainApplicationCMD = 0;


	return 0;
}

int ts_scan (int id, TW_SC_CONFIG config)
{
  ts_initialize();

  ts_connection_open(id); 

  set_config(&config);

  print_config();

  EnableDS_Short(config.gui_enable);

  ts_connection_close();

	return 0;
}

int ts_get_path ()
{
  string s = getexepath();
  wstring widestr = wstring(s.begin(), s.end());
  MessageBox(0, widestr.c_str(), L"str", MB_OK);

  return 0;
}

int ts_get_config (int id, TW_SC_CONFIG * config)
{
  config->gamma = (float)1.8;
  config->gui_enable = true;
  config->resolution = 200;

  return 0;
}

int ts_get_roi (int id, TW_SC_ROI * roi)
{
  ts_initialize();

  tw_ostream tw_cout;
  tw_cout << "Get ROI";

  ts_connection_open(id); 
 
  // frames
  print_cap(&(gpTwainApplicationCMD->m_ICAP_FRAMES));

  TW_CAPABILITY * _ptw_capability = &(gpTwainApplicationCMD->m_ICAP_FRAMES);

  TW_MEMREF pVal;
  pVal = _DSM_LockMemory(_ptw_capability->hContainer);

  pTW_FRAME pframe = (pTW_FRAME)&((pTW_ONEVALUE)pVal)->Item;

  roi->roi_left = FIX32ToFloat(pframe->Left);
  roi->roi_right = FIX32ToFloat(pframe->Right);
  roi->roi_top = FIX32ToFloat(pframe->Top);
  roi->roi_bottom = FIX32ToFloat(pframe->Bottom);

  ts_connection_close();

  return 0;
}


void set_config (TW_SC_CONFIG * _config)
{ 
  tw_cout << "---- set caps ----" << tw_endl;

  pTW_FIX32  pValFix32;
  TW_FIX32  valFix32 = {0};

  // xfermech
  if (_config->xfermech != -1)
  {
    tw_cout << "xfermech = " << _config->xfermech << tw_endl;
    gpTwainApplicationCMD->set_ICAP_XFERMECH((TW_UINT16) _config->xfermech);
  }

  print_cap(&(gpTwainApplicationCMD->m_ICAP_XFERMECH));

  // pixeltype
  if (_config->pixeltype != -1)
  {
    tw_cout << "pixeltype = " << _config->pixeltype << tw_endl;
    gpTwainApplicationCMD->set_ICAP_PIXELTYPE((TW_UINT16) _config->pixeltype);
  }

  // bitdepth
  if (_config->bitdepth != -1)
  {
    tw_cout << "bitdepth = " << _config->bitdepth << tw_endl;
    gpTwainApplicationCMD->set_ICAP_BITDEPTH((TW_UINT16) _config->bitdepth);
  }

  // imagefileformat
  if (_config->imagefileformat != -1)
  {
    tw_cout << "imagefileformat = " << _config->imagefileformat << tw_endl;
    gpTwainApplicationCMD->set_ICAP_IMAGEFILEFORMAT((TW_UINT16) _config->imagefileformat);
  }

  // units
  if (_config->units != -1)
  {
    tw_cout << "units = " << _config->units << tw_endl;
    gpTwainApplicationCMD->set_ICAP_UNITS((TW_UINT16) _config->units);
  }

  // resolution
  if (_config->resolution != -1)
  {
    tw_cout << "resolution = " << _config->resolution << tw_endl;
    valFix32 = FloatToFIX32(_config->resolution);
    pValFix32 = &valFix32;
    gpTwainApplicationCMD->set_ICAP_RESOLUTION(pValFix32);
  }

  // frames
  if (_config->roi_top >= 0 &&
      _config->roi_left >= 0 &&
      _config->roi_bottom > _config->roi_top &&
      _config->roi_right > _config->roi_left)
  {
    tw_cout << "frames = " 
            << _config->roi_top << " ," 
            << _config->roi_left << " ,"
            << _config->roi_bottom << " ,"
            << _config->roi_right << tw_endl;

    pTW_FRAME _pFrame;
    TW_FRAME _frame;
    _frame.Top = FloatToFIX32(_config->roi_top);
    _frame.Left = FloatToFIX32(_config->roi_left);
    _frame.Bottom = FloatToFIX32(_config->roi_bottom);
    _frame.Right = FloatToFIX32(_config->roi_right);
    _pFrame = &_frame;
    gpTwainApplicationCMD->set_ICAP_FRAMES(_pFrame);
  }

  // gamma
  if (_config->gamma != -1)
  {
    tw_cout << "gamma = " << _config->gamma << tw_endl;
    valFix32 = FloatToFIX32(_config->gamma);
    pValFix32 = &valFix32;
    gpTwainApplicationCMD->set_ICAP_GAMMA(pValFix32);
  }

  // brightness
  if (_config->brightness != -1)
  tw_cout << "brightness = " << _config->brightness << tw_endl;
  valFix32 = FloatToFIX32((float)_config->brightness);
  pValFix32 = &valFix32;
  gpTwainApplicationCMD->set_ICAP_BRIGHTNESS(pValFix32);

  // contrast
  if (_config->contrast != -1)
  tw_cout << "contrast = " << _config->contrast << tw_endl;
  valFix32 = FloatToFIX32((float)_config->contrast);
  pValFix32 = &valFix32;
  gpTwainApplicationCMD->set_ICAP_CONTRAST(pValFix32);


  return;
}

void get_config ()
{
  
}

void print_config ()
{
  // xfermech
  print_cap(&(gpTwainApplicationCMD->m_ICAP_XFERMECH));

  // pixeltype
  print_cap(&(gpTwainApplicationCMD->m_ICAP_PIXELTYPE));

  // bitdepth
  print_cap(&(gpTwainApplicationCMD->m_ICAP_BITDEPTH));

  // imagefileformat
  print_cap(&(gpTwainApplicationCMD->m_ICAP_IMAGEFILEFORMAT));

  // units
  print_cap(&(gpTwainApplicationCMD->m_ICAP_UNITS));

  // xresolution
  print_cap(&(gpTwainApplicationCMD->m_ICAP_XRESOLUTION));

  // yresolution
  print_cap(&(gpTwainApplicationCMD->m_ICAP_YRESOLUTION));

  // colormanagement enable
  print_cap(&(gpTwainApplicationCMD->m_CAP_COLORMANAGEMENTENABLED));

  // gamma
  print_cap(&(gpTwainApplicationCMD->m_ICAP_GAMMA));

  // brightness
  print_cap(&(gpTwainApplicationCMD->m_ICAP_BRIGHTNESS));

  // contrast
  print_cap(&(gpTwainApplicationCMD->m_ICAP_CONTRAST));

  // frames
  print_cap(&(gpTwainApplicationCMD->m_ICAP_FRAMES));

  // colormanagement enable
  print_cap(&(gpTwainApplicationCMD->m_CAP_COLORMANAGEMENTENABLED));

  // undefined image size
  print_cap(&(gpTwainApplicationCMD->m_ICAP_UNDEFINEDIMAGESIZE));

  // automatic border detection
  print_cap(&(gpTwainApplicationCMD->m_ICAP_AUTOMATICBORDERDETECTION));

  // autobright
  print_cap(&(gpTwainApplicationCMD->m_ICAP_AUTOBRIGHT));

  // bitorder
  print_cap(&(gpTwainApplicationCMD->m_ICAP_BITORDER));

  return;
}



void print_cap (TW_CAPABILITY * _ptw_capability)
{
  //TwainApp *_twainApp = new TwainApp();
  TW_MEMREF pVal;

  pVal = _DSM_LockMemory(_ptw_capability->hContainer);

  switch(_ptw_capability->ConType)
  {
    case TWON_ENUMERATION:
        print_ICAP(_ptw_capability->Cap, (pTW_ENUMERATION)(pVal));
    break;
    case TWON_ONEVALUE:
        print_ICAP(_ptw_capability->Cap, (pTW_ONEVALUE)(pVal));
      break;
    case TWON_RANGE:
        print_ICAP(_ptw_capability->Cap, (pTW_RANGE)(pVal));
      break;
    default:
      tw_cerr << convertCAP_toString(_ptw_capability->Cap)
              << " : print_cap() does not support this ConType: " 
              << _ptw_capability->ConType 
              << tw_endl;
  }

  _DSM_UnlockMemory(_ptw_capability->hContainer);

  return;
}

void ts_initialize()
{

  // initialize the log path
  string path = getexepath();
  path.append("TwainScan.log");
  tw_log_path = path;
}

string getexepath()
{
  WCHAR wc_result[ MAX_PATH ];
  char c_result[ MAX_PATH ];

  // get complete path with executeable file name
  GetModuleFileName( NULL, wc_result, MAX_PATH );

  //convert to char 
  wcstombs(c_result, wc_result, MAX_PATH);

  // convert to string
  string s_result = c_result;

  // cut the executable file name
  s_result = s_result.substr(0, s_result.find_last_of("\\") + 1);

  return s_result;
}