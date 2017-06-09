#ifndef __TWAINCOUT_H__
#define __TWAINCOUT_H__

// just a dummy class to overload cout and cerr
class tw_ostream{};


tw_ostream &operator<< (tw_ostream &ostr, char * a);

tw_ostream &operator<< (tw_ostream &ostr, const char * a);



#endif /*__TWAINCOUT_H__*/