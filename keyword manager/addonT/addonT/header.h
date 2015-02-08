#ifndef HEADER_H
#define HEADER_H

#include<map>
#include <vector>
#include <time.h>
#include <string>




#define CHECK(eval) if (! eval) { \
   const char *e = tagger ? tagger->what() : MeCab::getTaggerError(); \
   cerr << "Exception:" << e << endl; \
   delete tagger; \
   return -1; }


#endif