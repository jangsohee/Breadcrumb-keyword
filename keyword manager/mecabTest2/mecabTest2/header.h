#ifndef HEADER_H
#define HEADER_H

#include<map>
#include <vector>
#include <time.h>
#include <string>

using namespace std;





#define CHECK(eval) if (! eval) { \
   const char *e = tagger ? tagger->what() : MeCab::getTaggerError(); \
   std::cerr << "Exception:" << e << std::endl; \
   delete tagger; \
   return -1; }


#endif