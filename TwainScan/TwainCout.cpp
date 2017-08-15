
#include "TwainCout.h"
#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <stdio.h>

#include "TwainApp.h"
#include "CTiffWriter.h"
#include "TwainString.h"

using namespace std;

tw_ostream tw_cout;
tw_ostream tw_cerr;
char * tw_endl = "\n\x0";

void _write(char * a)
{
  ofstream myfile;
  myfile.open ("log.txt", fstream::app);
  myfile << a;
  myfile.close();
}

void _write(string a)
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

tw_ostream &operator<< (tw_ostream &ostr, string a)
{
  _write(a);

  return ostr;
}

tw_ostream &operator<< (tw_ostream &ostr, int a)
{
  char str [100];
  _itoa(a,str,10);
  _write(str);

  return ostr;
}

tw_ostream &operator<< (tw_ostream &ostr, short a)
{
  char str [100];
  _itoa(a,str,10);
  _write(str);

  return ostr;
}

tw_ostream &operator<< (tw_ostream &ostr, unsigned short a)
{
  char str [100];
  _itoa(a,str,10);
  _write(str);

  return ostr;
}

tw_ostream &operator<< (tw_ostream &ostr, float a)
{
  char str [100];
  sprintf(str, "%f", a);
  str[49] = '\0';
  _write(str);

  return ostr;
}

tw_ostream &operator<< (tw_ostream &ostr, unsigned long a)
{
  char str [100];
  sprintf(str, "%lu", a); 
  _write(str);

  return ostr;
}

tw_ostream &operator<< (tw_ostream &ostr, long a)
{
  char str [100];
  sprintf(str, "%l", a); 
  _write(str);

  return ostr;
}