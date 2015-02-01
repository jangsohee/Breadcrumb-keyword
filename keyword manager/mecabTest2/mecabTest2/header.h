#ifndef HEADER_H
#define HEADER_H

#include<map>
#include <vector>
#include <time.h>
#include <string>

using namespace std;

map<pair<string, string>, int> stringTypeNum;
multimap<int, pair<string, string> > numStringType;
vector<string> vs;


#define CHECK(eval) if (! eval) { \
   const char *e = tagger ? tagger->what() : MeCab::getTaggerError(); \
   std::cerr << "Exception:" << e << std::endl; \
   delete tagger; \
   return -1; }


//현재 시간 구하기.
time_t timer = time(NULL);
tm* t = localtime(&timer);

string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);

string mecabInputTXT = "../../../../keyword manager DB/1.input/input.txt";
string mecabOutputTXT = "../../../../keyword manager DB/2.mecab_output/" + thisTime + "output.txt";
string mecabAnalyzeTXT = "../../../../keyword manager DB/3.final/" + thisTime + "analyzeResult.txt";
string mecabTotalTFTXT = "../../../../keyword manager DB/4.total_TF_input/00.total_TF_input.txt";

#endif