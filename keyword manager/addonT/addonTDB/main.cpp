#include <node.h>
#include <iostream>
#include <Windows.h>
#include <map>
#include <string>
#include <fstream>
#include <math.h>
#include <limits>
#include <time.h>

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

	cout << "***  강석일 회원의 keyword DB를 쌓겠습니다..  ***" << endl;


	WSADATA wsaData;
	SOCKET hSocket;
	char msg[100];
	int strlen;
	SOCKADDR_IN servAdr;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	hSocket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(PORT);
	servAdr.sin_addr.s_addr = inet_addr(SERVERIP);

	if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		cout << "connect error" << endl;
	else
		cout << "connected!" << endl;




	double value = args[0]->NumberValue() + args[1]->NumberValue();
	Local<Number> num = Number::New(isolate, value);

	args.GetReturnValue().Set(num);
}

void Init(Handle<Object> exports) {
	NODE_SET_METHOD(exports, "add", Add);
}

NODE_MODULE(addon, Init)