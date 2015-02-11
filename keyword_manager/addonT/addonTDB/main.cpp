#include <node.h>
#include <iostream>
#include <Windows.h>
#include <map>
#include <string>
#include <fstream>
#include <math.h>
#include <limits>
#include <time.h>
#include <vector>

#pragma comment(lib, "node.lib")

using namespace v8;
using std::map;
using std::pair;
using std::string;
using std::multimap;
using std::to_string;
using std::cerr;
using std::endl;
using std::cout;
using std::getline;
using std::ifstream;
using std::ofstream;
using std::vector;

#define SERVERIP "127.0.0.1"
#define PORT 74132

map<pair<string, string>, double> stringTypeNum;

void Add(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	ifstream iDB("./01.total_TF_result.txt");

	int historyTotalNum;
	iDB >> historyTotalNum;
	
	string tmp;
	getline(iDB, tmp, '\n');
	
	vector<string> nn;
	vector<string> nt;
	vector<int>ff;
	while (1)
	{
		string noun;
		getline(iDB, noun, '\t');
		if (iDB.eof()) break;
		nn.push_back(noun);

		string type;
		iDB >> type;
		nt.push_back(type);

		int DF;
		iDB >> DF;
		ff.push_back(DF);

		getline(iDB, tmp, '\n');
	}
	cout << "?";
	int size = nn.size();
	Local<Array> noun = Array::New(isolate, size);
	Local<Array> nounType = Array::New(isolate, size);
	Local<Array> DF = Array::New(isolate, size);

	for (int i=0; i<size; ++i)
	{ 
		noun->Set(i, String::NewFromUtf8(isolate, nn[i].c_str()));
		nounType->Set(i, String::NewFromUtf8(isolate, nt[i].c_str()));
		DF->Set(i, Number::New(isolate, ff[i]));
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "historyTotalNum"), Number::New(isolate, historyTotalNum));
	obj->Set(String::NewFromUtf8(isolate, "noun"), noun);
	obj->Set(String::NewFromUtf8(isolate, "nounType"), nounType);
	obj->Set(String::NewFromUtf8(isolate, "TF"), DF);
	//obj->Set(String::NewFromUtf8(isolate, "ParentKeyword"), String::NewFromUtf8(isolate, parentKeyword.c_str()));

	args.GetReturnValue().Set(obj);
}

void Init(Handle<Object> exports) {
	NODE_SET_METHOD(exports, "add", Add);
}

NODE_MODULE(addon, Init)