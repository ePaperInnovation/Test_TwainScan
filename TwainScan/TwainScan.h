


//#include "stdafx.h"
//


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
* @file main.h
* main header file the common includes and defines
* @author TWAIN Working Group
* @date October 2007
*/
#ifndef __MAIN_H__
#define __MAIN_H__

enum ENUM_TS_ACTION
{
  TS_SCAN = 0,
  TS_GET_CONFIG =1
};

typedef struct 
{
  bool gui_enable;
  int xfermech;
  int pixeltype;
  int bitdepth;
  int imagefileformat;
  int units;
  float resolution;
  float roi_top;
  float roi_left;
  float roi_bottom;
  float roi_right;
  int brightness;
  int contrast;
  float gamma;
  bool log_enable;
} TW_SC_CONFIG;

extern "C" __declspec(dllexport) double Add_hokus(double a, double b);
extern "C" __declspec(dllexport) double Add_hokus2(double a, double b);

int ts_action (int id, ENUM_TS_ACTION ts_action);
int ts_connection_open(int id);
int ts_connection_close();
void ts_initialize();

extern "C" __declspec(dllexport) int ts_select(int *id);
extern "C" __declspec(dllexport) int ts_scan(int id, TW_SC_CONFIG config);
extern "C" __declspec(dllexport) int ts_get_config(int id, TW_SC_CONFIG *config);
extern "C" __declspec(dllexport) int ts_get_path();


#include "Common.h"
#include "CommonTWAIN.h"
#include "TwainApp.h"
#include "twain.h"

void set_config (TW_SC_CONFIG * _config);
void print_config();
void print_cap (TW_CAPABILITY * _ptw_capability);

string getexepath();

//void negotiate_CAP(const pTW_CAPABILITY _pCap);


#ifdef TWH_CMP_MSC
  #include <tchar.h>

  // TODO: reference additional headers your program requires here
  #include <io.h>
  #include <iostream>

  void printWindowsErrorMessage(); // defined in Twain_DS_sample01.cpp


#else
  #include <stdlib.h>
  #include <iostream>

#endif // TWH_CMP_MSC






#endif //__MAIN_H__