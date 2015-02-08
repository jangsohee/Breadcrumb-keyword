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
using std::cin;
using std::ofstream;
using std::ifstream;


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


	cout << "***  keyword analyzer(MECAB)  ***" << endl;


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




	while (1)
	{
		cout << "START?";
		string tmp;
		cin >> tmp;
		if (tmp == "n") break;

		_finddata_t fd;
		long handle;
		int chkResult = 1;
		//handle = _findfirst("../../../../datamaker/realCrawling/DB/*.*", &fd);
		handle = _findfirst("../../../../keyword manager DB1/1.input/*.*", &fd);

		if (handle == -1)
		{
			cout << "파일 읽기 오류" << endl;
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong arguments")));
			return ;
		}

		while (chkResult != -1)
		{
			string fdName = fd.name;

			if (fdName == "." || fdName == "..")
			{
				chkResult = _findnext(handle, &fd);
				continue;
			}

			vector<string> vs;
			map<pair<string, string>, int> stringTypeNum;
			multimap<int, pair<string, string> > numStringType;

			cout << fdName << endl;
			//cout << "step1?"<<fdName ;
			//string tmp;
			//cin >> tmp;
			//if (tmp == "n") break;


			time_t timer = time(NULL);
			tm* t = localtime(&timer);

			string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
				+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);

			//string mecabInputTXT = "../../../../datamaker/realCrawling/DB/" + fdName;
			string mecabInputTXT = "../../../../keyword manager DB1/1.input/" + fdName;
			string mecabOutputTXT = "../../../../keyword manager DB1/2.mecab_output/" + thisTime + "output.txt";
			string mecabAnalyzeTXT = "../../../../keyword manager DB1/3.final/" + thisTime + "analyzeResult.txt";
			string mecabTotalTFTXT = "../../../../keyword manager DB1/4.total_TF_input/00.total_TF_input.txt";




			//mecab engine 구동
			MeCab::Tagger *tagger = MeCab::createTagger("-r C:/librarys/mecab-ko/mecabrc -d C:/librarys/mecab-ko/dic/mecab-ko-dic-1.6.1");
			//CHECK(tagger);

			ifstream ImecabInput(mecabInputTXT);
			ofstream OmecabOutput(mecabOutputTXT);



			while (1)
			{
				string input;

				getline(ImecabInput, input, '\n');
				if (ImecabInput.eof())
				{
					const char *result = tagger->parse(input.c_str());
					//CHECK(result);

					OmecabOutput << result << endl;
					break;
				}

				if (input == "") continue;

				// Gets tagged result in string format.
				const char *result = tagger->parse(input.c_str());
				//CHECK(result);

				OmecabOutput << result << endl;

			}

			delete tagger;

			ImecabInput.close();
			OmecabOutput.close();


			//-----------------------분석모듈-------------------------------------------------
			ifstream ImecabOutput(mecabOutputTXT);
			ofstream OmecabAnalyze(mecabAnalyzeTXT);

			string apstr;
			bool continuous = false;
			int cnt = 0;

			while (1)
			{
				string line;
				getline(ImecabOutput, line, '\n');
				if (ImecabOutput.eof())
				{
					if (continuous == true && cnt != 1)
					{
						OmecabAnalyze << endl;
						continuous = false;

						for (int i = 0; i < cnt; ++i)
						{
							int j = apstr.find(" ");
							string tmp = apstr.substr(0, j);
							apstr.erase(0, j + 1);
							vs.push_back(tmp);
						}
						apstr.erase();

						//apstr 작업
						for (int i = 2; i <= cnt; ++i)
						{
							for (int k = 0; k + i <= vs.size(); ++k)
							{
								string appendStr;
								for (int j = 0; j < i; ++j)
								{
									appendStr += (vs[k + j] + " ");
								}
								stringTypeNum[make_pair(appendStr, "MyCompound")]++;
							}
						}
						vs.clear();
						cnt = 0;
					}
					else if (continuous == true && cnt == 1)
					{
						apstr.clear();
						cnt = 0;
						OmecabAnalyze << endl;
						continuous = false;
					}
					break;
				}
				if (line.empty() || line[0] == '\t' || line == "EOS")
				{
					if (continuous == true && cnt != 1)
					{
						OmecabAnalyze << endl;
						continuous = false;

						for (int i = 0; i < cnt; ++i)
						{
							int j = apstr.find(" ");
							string tmp = apstr.substr(0, j);
							apstr.erase(0, j + 1);
							vs.push_back(tmp);
						}
						apstr.erase();

						//apstr 작업
						for (int i = 2; i <= cnt; ++i)
						{
							for (int k = 0; k + i <= vs.size(); ++k)
							{
								string appendStr;
								for (int j = 0; j < i; ++j)
								{
									appendStr += (vs[k + j] + " ");
								}
								stringTypeNum[make_pair(appendStr, "MyCompound")]++;
							}
						}
						vs.clear();
						cnt = 0;
					}
					else if (continuous == true && cnt == 1)
					{
						apstr.clear();
						cnt = 0;
						OmecabAnalyze << endl;
						continuous = false;
					}
					continue;
				}

				size_t pos = line.find('\t');
				string noun = line.substr(0, pos);

				if (noun == "﻿")
				{
					cout << "?" << line << endl;
				}

				size_t dotPos = line.find(',', pos + 1);
				string ty = line.substr(pos + 1, dotPos - pos - 1);




				// 영어 단어의 비율 체크 해보자.
				// 총 단어도 같이 세어보자. tf모델의 변형 가능성 열어두기 위함.

				if ((ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP") && continuous == true)
				{
					OmecabAnalyze << noun << '\t' << ty << endl;
					stringTypeNum[make_pair(noun, ty)]++;
					apstr.append(noun + " ");
					cnt++;
				}
				else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP")
				{
					OmecabAnalyze << noun << '\t' << ty << endl;
					stringTypeNum[make_pair(noun, ty)]++;
					continuous = true;
					apstr.append(noun + " ");
					cnt++;
				}
				else if (ty == "SL" && continuous == true)
				{
					OmecabAnalyze << noun << '\t' << ty << endl;
					stringTypeNum[make_pair(noun, ty)]++;
					continuous = false;

					//vs벡터에 단어 하나하나 저장.
					for (int i = 0; i < cnt; ++i)
					{
						int j = apstr.find(" ");
						string tmp = apstr.substr(0, j);
						apstr.erase(0, j + 1);
						vs.push_back(tmp);
					}
					apstr.erase();

					//apstr 작업. 복합명사 만들어서 세어보는중.
					for (int i = 2; i <= cnt; ++i)
					{
						for (int k = 0; k + i <= vs.size(); ++k)
						{
							string appendStr;
							for (int j = 0; j < i; ++j)
							{
								appendStr += (vs[k + j] + " ");
							}
							stringTypeNum[make_pair(appendStr, "MyCompound")]++;
						}
					}
					vs.clear();
					cnt = 0;

				}
				else if (ty == "SL")
				{
					OmecabAnalyze << noun << '\t' << ty << endl;
					stringTypeNum[make_pair(noun, ty)]++;
				}
				else if (continuous == true && cnt != 1)
				{
					OmecabAnalyze << endl;
					continuous = false;

					for (int i = 0; i < cnt; ++i)
					{
						int j = apstr.find(" ");
						string tmp = apstr.substr(0, j);
						apstr.erase(0, j + 1);
						vs.push_back(tmp);
					}
					apstr.erase();

					//apstr 작업
					for (int i = 2; i <= cnt; ++i)
					{
						for (int k = 0; k + i <= vs.size(); ++k)
						{
							string appendStr;
							for (int j = 0; j < i; ++j)
							{
								appendStr += (vs[k + j] + " ");
							}
							stringTypeNum[make_pair(appendStr, "MyCompound")]++;
						}
					}
					vs.clear();
					cnt = 0;
				}
				else
				{
					OmecabAnalyze << endl;
					continuous = false;
					cnt = 0;
					apstr.erase();
				}
			}

			OmecabAnalyze << "----------------------------------------------------------------------------------" << endl;

			string ans;
			double maxx = 0;
			map<pair<string, string>, int>::iterator it;
			for (it = stringTypeNum.begin(); it != stringTypeNum.end(); ++it)
			{
				OmecabAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
				if (maxx < (*it).second)
				{
					maxx = (*it).second;
					ans = (*it).first.first;
				}
				numStringType.insert(pair<int, pair<string, string> >((*it).second, (*it).first));
			}

			OmecabAnalyze << endl << "----------------------------------------------------------------------------------" << endl;
			OmecabAnalyze << "keyword : " << ans;
			OmecabAnalyze << endl << "----------------------------------------------------------------------------------";
			OmecabAnalyze << endl;

			ofstream OmecabTotalTF(mecabTotalTFTXT);

			string mecabCountTXT = "../../../../keyword manager DB1/3-0.simple_count_TF/00.countTFAnalyzeResult.txt";//  *
			string mecabBooleanTFTXT = "../../../../keyword manager DB1/3-1.boolean_TF/00.booleanTFAnalyzeResult.txt";
			string mecabLogTXT = "../../../../keyword manager DB1/3-2.log_TF/00.logTFAnalyzeResult.txt";
			string mecabIncreaseTXT = "../../../../keyword manager DB1/3-3.increase_TF/00.increaseTFAnalyzeResult.txt";

			double totalNoun = 0;
			double totalEnglishNoun = 0;


			ofstream OCountTXT(mecabCountTXT);
			ofstream OBooleanTFTXT(mecabBooleanTFTXT);
			ofstream OLogTXT(mecabLogTXT);
			ofstream OIncreaseTXT(mecabIncreaseTXT);

			map<pair<string, string>, double > countMap;
			map<pair<string, string>, double > booleanMap;
			map<pair<string, string>, double > logMap;
			map<pair<string, string>, double > increaseMap;



			multimap<int, pair<string, string> >::iterator it2;
			for (it2 = numStringType.begin(); it2 != numStringType.end(); ++it2){

				OmecabAnalyze << (*it2).second.first << '\t' << (*it2).second.second << '\t' << (*it2).first << endl;


				countMap.insert(make_pair(make_pair((*it2).second.first, (*it2).second.second), (*it2).first));
				booleanMap.insert(make_pair(make_pair((*it2).second.first, (*it2).second.second), 1));

				double logTF = log10(((*it2).first) + 1);
				logMap.insert(make_pair(make_pair((*it2).second.first, (*it2).second.second), logTF));

				double increaseTF = 0.5 + (0.5 * (*it2).first) / maxx;
				increaseMap.insert(make_pair(make_pair((*it2).second.first, (*it2).second.second), increaseTF));


				//OCountTXT << (*it2).second.first << '\t' << (*it2).second.second << '\t' << (*it2).first << endl;
				OmecabTotalTF << (*it2).second.first << '\t' << (*it2).second.second << '\t' << 1 << endl;
				//OBooleanTFTXT << (*it2).second.first << '\t' << (*it2).second.second << '\t' << 1 << endl;

				totalNoun += (*it2).first;
				if ((*it2).second.second == "SL") totalEnglishNoun += (*it2).first;
			}



			double englishP = totalEnglishNoun / totalNoun;
			OmecabAnalyze << "total : " << totalNoun << '\t' << "english : " << totalEnglishNoun << '\t' << "english/total : " << englishP << endl;


			ImecabOutput.close();
			OmecabAnalyze.close();
			OmecabTotalTF.close();
			OCountTXT.close();
			OBooleanTFTXT.close();
			OLogTXT.close();
			OIncreaseTXT.close();






			//---------------------------------------------------indexing모듈 완료----------------------------------------------------------------------
			char msg[100] = "1";
			send(clntSock, msg, strlen(msg), 0);

			int len = recv(clntSock, msg, 99, 0);
			msg[len] = 0;

			cout << "step2?" << endl;


			//IDF calculate

			multimap<double, pair<string, string> > cTFIDFNounType;
			multimap<double, pair<string, string> > bTFIDFNounType;
			multimap<double, pair<string, string> > lTFIDFNounType;
			multimap<double, pair<string, string> > iTFIDFNounType;


			string mecabSelectivDF = "../../../../keyword manager DB1/5-1.selectiveDF/00.selectve.txt";

			ifstream ISelectiveTDF(mecabSelectivDF);

			int documentN;

			ISelectiveTDF >> documentN;
			string ttmp;
			getline(ISelectiveTDF, ttmp, '\n');
			while (1)
			{
				string noun, ty;


				double DF;
				getline(ISelectiveTDF, noun, '\t');
				if (ISelectiveTDF.eof()) break;
				ISelectiveTDF >> ty >> DF;

				double countTF, booleanTF, LogTF, IncreaseTF;
				countTF = countMap[make_pair(noun, ty)];
				booleanTF = booleanMap[make_pair(noun, ty)];
				LogTF = logMap[make_pair(noun, ty)];
				IncreaseTF = increaseMap[make_pair(noun, ty)];


				//해당 단어의 IDF값을 구하고,

				double IDF = log10(documentN / DF);

				// 상황에 맞는 TF와 곱한다.
				double cTFIDF = countTF * IDF;
				double bTFIDF = booleanTF * IDF;
				double lTFIDF = LogTF * IDF;
				double iTFIDF = IncreaseTF * IDF;

				cTFIDFNounType.insert(make_pair(cTFIDF, make_pair(noun, ty)));
				bTFIDFNounType.insert(make_pair(bTFIDF, make_pair(noun, ty)));
				lTFIDFNounType.insert(make_pair(lTFIDF, make_pair(noun, ty)));
				iTFIDFNounType.insert(make_pair(iTFIDF, make_pair(noun, ty)));

				string ttmp;

				getline(ISelectiveTDF, ttmp, '\n');
			}

			string simpleCountTFIDFResult = "../../../../keyword manager DB1/7-0.simple_count_TF-IDF/" + thisTime + " SimpleCountTFIDFAnalyzeResult.txt";
			string BooleanTFIDFResult = "../../../../keyword manager DB1/7-1.boolean_TF-IDF/" + thisTime + " BooleanTFIDFAnalyzeResult.txt";
			string LogTFIDFResult = "../../../../keyword manager DB1/7-2.log_TF-IDF/" + thisTime + " LogTFIDFAnalyzeResult.txt";
			string IncreaseTFIDFResult = "../../../../keyword manager DB1/7-3.increase_TF-IDF/" + thisTime + " IncreaseTFIDFAnalyzeResult.txt";

			ofstream OCountTFIDF(simpleCountTFIDFResult);
			ofstream OBooleanTFIDF(BooleanTFIDFResult);
			ofstream OLogTFIDF(LogTFIDFResult);
			ofstream OIncreaseTFIDF(IncreaseTFIDFResult);


			multimap<double, pair<string, string> >::reverse_iterator it3;
			for (it3 = cTFIDFNounType.rbegin(); it3 != cTFIDFNounType.rend(); ++it3)
			{
				OCountTFIDF << it3->second.first << '\t' << it3->second.second << '\t' << it3->first << endl;
			}

			for (it3 = bTFIDFNounType.rbegin(); it3 != bTFIDFNounType.rend(); ++it3)
			{
				OBooleanTFIDF << it3->second.first << '\t' << it3->second.second << '\t' << it3->first << endl;
			}

			for (it3 = lTFIDFNounType.rbegin(); it3 != lTFIDFNounType.rend(); ++it3)
			{
				OLogTFIDF << it3->second.first << '\t' << it3->second.second << '\t' << it3->first << endl;
			}

			for (it3 = iTFIDFNounType.rbegin(); it3 != iTFIDFNounType.rend(); ++it3)
			{
				OIncreaseTFIDF << it3->second.first << '\t' << it3->second.second << '\t' << it3->first << endl;
			}


			chkResult = _findnext(handle, &fd);
		}

		char msg[100] = "0";
		send(clntSock, msg, strlen(msg), 0);
	}
	









	double value = args[0]->NumberValue() + args[1]->NumberValue();
	Local<Number> num = Number::New(isolate, value);
	
	Local<String> str = String::NewFromUtf8(isolate, "성공!");
	//std::string ddd = "IUZZANG";
	args.GetReturnValue().Set(str);
	//args.GetReturnValue().Set(num);

}

void Init(Handle<Object> exports) {
	NODE_SET_METHOD(exports, "add", Add);
}

NODE_MODULE(addon, Init)