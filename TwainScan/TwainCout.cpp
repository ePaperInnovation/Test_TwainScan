
#include "TwainCout.h"
#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <stdio.h>

#include "TwainApp.h"
#include "CTiffWriter.h"
#include "TwainString.h"

using namespace std;

void _write(char * a)
{
  ofstream myfile;
  myfile.open ("log.txt", fstream::app);
  myfile << a;
  myfile.close();
}

tw_ostream &operator<< (tw_ostream &ostr, char * a)
{
  _write(a);

  return ostr;
}

tw_ostream &operator<< (tw_ostream &ostr, const char * a)
{
  _write((char*) a);

  return ostr;
}



