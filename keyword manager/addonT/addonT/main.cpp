#include <v8.h>
#include <node.h>
#include <iostream>
#include <WinSock2.h>
#include <fstream>
#include <string>
#include <Windows.h>
#include <cstdio>
#include <map>
#include <vector>
#include <algorithm>
#include <time.h>
#include <math.h>
#include <io.h>
#include <conio.h>
#include <mecab.h>

#include "header.h"

#pragma comment(lib, "node.lib")

using namespace v8;
using std::vector;
using std::map;
using std::pair;
using std::string;
using std::multimap;
using std::to_string;
using std::cerr;
using std::endl;
using std::cout;

#define SERVERIP "127.0.0.1"
#define PORT 74132



void Add(const FunctionCallbackInfo<Value>& args) {

	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	if (args.Length() < 2) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}


	//--------------------------------------------------


	std::cout << "***  keyword analyzer(MECAB)  ***" << std::endl;
	

	WSADATA wsaData;
	SOCKET hSocket, clntSock;
	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSize;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	hSocket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(PORT);

	bind(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr));

	listen(hSocket, 5);

	clntAdrSize = sizeof(clntAdr);
	clntSock = accept(hSocket, (SOCKADDR*)&servAdr, &clntAdrSize);

	if (clntSock == -1)
		cout << "socket error" << endl;
	else
		cout << "connected!" << endl;


	vector<string> vs;
	map<pair<string, string>, int> stringTypeNum;
	multimap<int, pair<string, string> > numStringType;


	time_t timer = time(NULL);
	tm* t = localtime(&timer);

	string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
		+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);


	MeCab::Tagger *tagger = MeCab::createTagger("-r C:/librarys/mecab-ko/mecabrc -d C:/librarys/mecab-ko/dic/mecab-ko-dic-1.6.1");
	//CHECK(tagger);
	

	string mecabInputTXT = "../../../../keyword manager DB/1.input/input.txt";
	string mecabOutputTXT = "../../../../keyword manager DB/2.mecab_output/" + thisTime + "output.txt";
	string mecabAnalyzeTXT = "../../../../keyword manager DB/3.final/" + thisTime + "analyzeResult.txt";
	string mecabTotalTFTXT = "../../../../keyword manager DB/4.total_TF_input/00.total_TF_input.txt";

	string input;
	const char *result = tagger->parse(input.c_str());









	double value = args[0]->NumberValue() + args[1]->NumberValue();
	Local<Number> num = Number::New(isolate, value);
	
	Local<String> str = String::NewFromUtf8(isolate, "dd");
	//std::string ddd = "IUZZANG";
	args.GetReturnValue().Set(str);
	//args.GetReturnValue().Set(num);

}

void Init(Handle<Object> exports) {
	NODE_SET_METHOD(exports, "add", Add);
}

NODE_MODULE(addon, Init)