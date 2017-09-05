#ifndef __TWAINCOUT_H__
#define __TWAINCOUT_H__

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <stdio.h>

#include "TwainApp.h"
#include "CTiffWriter.h"
#include "TwainString.h"
#include "TwainCout.h"

// just a dummy class to overload tw_cout and tw_cerr
class tw_ostream{};

tw_ostream &operator<< (tw_ostream &ostr, char * a);

tw_ostream &operator<< (tw_ostream &ostr, const char * a);

tw_ostream &operator<< (tw_ostream &ostr, string a);

tw_ostream &operator<< (tw_ostream &ostr, short a);

tw_ostream &operator<< (tw_ostream &ostr, unsigned short);

tw_ostream &operator<< (tw_ostream &ostr, float a);

tw_ostream &operator<< (tw_ostream &ostr, long);

tw_ostream &operator<< (tw_ostream &ostr, unsigned long);

tw_ostream &operator<< (tw_ostream &ostr, int);

//tw_ostream &operator<< (tw_ostream &ostr, void a);

// tw_cout needs to be extern to have no linking problems
extern tw_ostream tw_cout;
extern tw_ostream tw_cerr;
extern char * tw_endl; //"\n\x0"
extern char * tw_log_path;
extern bool tw_log_enable;

#endif /*__TWAINCOUT_H__*/