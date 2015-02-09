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


///while문 다 빼버릴 것.


void CreateObject(const FunctionCallbackInfo<Value>& args) {

	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	/*if (args.Length() < 2) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}*/


	//------------------------------------------------------------------------------------------------------------


	cout << "***  keyword analyzer(MECAB)  ***" << endl;
	cout << "start!" << endl;

	time_t timer = time(NULL);
	tm* t = localtime(&timer);

	string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
		+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);


	//title과 body로 받아오게끔

	string mecabInputTXT = "../../../../keyword manager DB1/1.input/input.txt";
	ifstream ImecabInput(mecabInputTXT);

	string title;
	getline(ImecabInput, title, '\n');

	string body;
	getline(ImecabInput, body, '\0');

	ImecabInput.close();


	//------이부분 사실 매개변수로 올 것이다.
	//title = args[0]->ToString;
	//body = args[1]->ToString;





	//mecab engine 구동
	MeCab::Tagger *tagger = MeCab::createTagger("-r C:/librarys/mecab-ko/mecabrc -d C:/librarys/mecab-ko/dic/mecab-ko-dic-1.6.1");
	//CHECK(tagger);


	// *** 파일 출력 막아 놓았다. 확인 하려면 주석을 풀어야 ***


	string mecabOutputTXT = "../../../../keyword manager DB1/2.mecab_output/" + thisTime + "output.txt";
	ofstream OmecabOutput(mecabOutputTXT);


	// Gets tagged result in string format.
	const char *result = tagger->parse(title.c_str());
	//CHECK(result);
	string Atitle = result;
	OmecabOutput << Atitle << endl<<endl;

	const char *result2 = tagger->parse(body.c_str());
	string Abody = result2;
	OmecabOutput << Abody << endl;

	delete tagger;


	OmecabOutput.close();



	map<pair<string, string>, double> totalMap;
	//---------------------------------------분석모듈-------------------------------------------------



	map<pair<string, string>, int> titleMap;
	//title 부터 분석.
	{
		string titleAnalyzeTXT = "../../../../keyword manager DB1/3-0-0.final_title/" + thisTime + "analyzeResult.txt";
		ofstream OtitleAnalyze(titleAnalyzeTXT);

		vector<string> vs;

		string apstr;
		bool continuous = false;
		int cnt = 0;
		size_t found = 0, found2;

		while (1)
		{
			found2 = Atitle.find('\n', found);
			string line = Atitle.substr(found, found2 - found);

			if (found2 == std::string::npos || line == "EOS")
			{
				if (continuous == true && cnt != 1)
				{
					OtitleAnalyze << endl;
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
							titleMap[make_pair(appendStr, "MyCompound")]++;
						}
					}
					vs.clear();
					cnt = 0;
				}
				else if (continuous == true && cnt == 1)
				{
					apstr.clear();
					cnt = 0;
					OtitleAnalyze << endl;
					continuous = false;
				}
				break;
			}

			if (line.empty() || line[0] == '\t' || line == "EOS")
			{
				if (continuous == true && cnt != 1)
				{
					OtitleAnalyze << endl;
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
							titleMap[make_pair(appendStr, "MyCompound")]++;
						}
					}
					vs.clear();
					cnt = 0;
				}
				else if (continuous == true && cnt == 1)
				{
					apstr.clear();
					cnt = 0;
					OtitleAnalyze << endl;
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
				OtitleAnalyze << noun << '\t' << ty << endl;
				titleMap[make_pair(noun, ty)]++;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP")
			{
				OtitleAnalyze << noun << '\t' << ty << endl;
				titleMap[make_pair(noun, ty)]++;
				continuous = true;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "SL" && continuous == true)
			{
				OtitleAnalyze << noun << '\t' << ty << endl;
				titleMap[make_pair(noun, ty)]++;
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
						titleMap[make_pair(appendStr, "MyCompound")]++;
					}
				}
				vs.clear();
				cnt = 0;

			}
			else if (ty == "SL")
			{
				OtitleAnalyze << noun << '\t' << ty << endl;
				titleMap[make_pair(noun, ty)]++;
			}
			else if (continuous == true && cnt != 1)
			{
				OtitleAnalyze << endl;
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
						titleMap[make_pair(appendStr, "MyCompound")]++;
					}
				}
				vs.clear();
				cnt = 0;
			}
			else
			{
				OtitleAnalyze << endl;
				continuous = false;
				cnt = 0;
				apstr.erase();
			}


			found = found2 + 1;
		}

		OtitleAnalyze << "----------------------------------------------------------------------------------" << endl;


		// 타이틀 멀티맵에 값 넣기.
		// 과연 1번 모듈에선 타이틀 멀티맵이 필요한가?
		string ans;
		double maxx = 0;
		map<pair<string, string>, int>::iterator it;
		for (it = titleMap.begin(); it != titleMap.end(); ++it)
		{
			OtitleAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
			if (maxx < (*it).second)
			{
				maxx = (*it).second;
				ans = (*it).first.first;
			}
			//titleMultiMap.insert(pair<int, pair<string, string> >((*it).second, (*it).first));
		}

		OtitleAnalyze << endl << "----------------------------------------------------------------------------------" << endl;
		OtitleAnalyze << "keyword : " << ans;
		OtitleAnalyze << endl << "----------------------------------------------------------------------------------";
		OtitleAnalyze << endl;


		for (it = titleMap.begin(); it != titleMap.end(); ++it)
		{
			double increaseTF = 0.5 + (0.5 * (*it).second) / maxx;
			OtitleAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << increaseTF << endl;
			totalMap[make_pair((*it).first.first, (*it).first.second)] += increaseTF;
		}


		//------------------------------------title 분석 완료---------------------------------------------
	}


	map<pair<string, string>, int> bodyMap;
	//body 분석.
	{
		string bodyAnalyzeTXT = "../../../../keyword manager DB1/3-0-1.final_body/" + thisTime + "analyzeResult.txt";
		ofstream ObodyAnalyze(bodyAnalyzeTXT);

		vector<string> vs;
		
		//multimap<int, pair<string, string> > titleMultiMap;

		string apstr;
		bool continuous = false;
		int cnt = 0;
		size_t found = 0, found2;

		while (1)
		{
			found2 = Abody.find('\n', found);
			string line = Abody.substr(found, found2 - found);

			if (found2 == std::string::npos || line == "EOS")
			{
				if (continuous == true && cnt != 1)
				{
					ObodyAnalyze << endl;
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
							bodyMap[make_pair(appendStr, "MyCompound")]++;
						}
					}
					vs.clear();
					cnt = 0;
				}
				else if (continuous == true && cnt == 1)
				{
					apstr.clear();
					cnt = 0;
					ObodyAnalyze << endl;
					continuous = false;
				}
				break;
			}

			if (line.empty() || line[0] == '\t' || line == "EOS")
			{
				if (continuous == true && cnt != 1)
				{
					ObodyAnalyze << endl;
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
							bodyMap[make_pair(appendStr, "MyCompound")]++;
						}
					}
					vs.clear();
					cnt = 0;
				}
				else if (continuous == true && cnt == 1)
				{
					apstr.clear();
					cnt = 0;
					ObodyAnalyze << endl;
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
				ObodyAnalyze << noun << '\t' << ty << endl;
				bodyMap[make_pair(noun, ty)]++;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP")
			{
				ObodyAnalyze << noun << '\t' << ty << endl;
				bodyMap[make_pair(noun, ty)]++;
				continuous = true;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "SL" && continuous == true)
			{
				ObodyAnalyze << noun << '\t' << ty << endl;
				bodyMap[make_pair(noun, ty)]++;
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
						bodyMap[make_pair(appendStr, "MyCompound")]++;
					}
				}
				vs.clear();
				cnt = 0;

			}
			else if (ty == "SL")
			{
				ObodyAnalyze << noun << '\t' << ty << endl;
				bodyMap[make_pair(noun, ty)]++;
			}
			else if (continuous == true && cnt != 1)
			{
				ObodyAnalyze << endl;
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
						bodyMap[make_pair(appendStr, "MyCompound")]++;
					}
				}
				vs.clear();
				cnt = 0;
			}
			else
			{
				ObodyAnalyze << endl;
				continuous = false;
				cnt = 0;
				apstr.erase();
			}


			found = found2 + 1;
		}

		ObodyAnalyze << "----------------------------------------------------------------------------------" << endl;


		// 타이틀 멀티맵에 값 넣기.
		// 과연 1번 모듈에선 타이틀 멀티맵이 필요한가?
		string ans;
		double maxx = 0;
		map<pair<string, string>, int>::iterator it;
		for (it = bodyMap.begin(); it != bodyMap.end(); ++it)
		{
			ObodyAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
			if (maxx < (*it).second)
			{
				maxx = (*it).second;
				ans = (*it).first.first;
			}
			//titleMultiMap.insert(pair<int, pair<string, string> >((*it).second, (*it).first));
		}

		ObodyAnalyze << endl << "----------------------------------------------------------------------------------" << endl;
		ObodyAnalyze << "keyword : " << ans;
		ObodyAnalyze << endl << "----------------------------------------------------------------------------------";
		ObodyAnalyze << endl;


		for (it = bodyMap.begin(); it != bodyMap.end(); ++it)
		{
			double increaseTF = 0.5 + (0.5 * (*it).second) / maxx;
			ObodyAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << increaseTF << endl;
			totalMap[make_pair((*it).first.first, (*it).first.second)] += increaseTF;
		}


	}


	





	/*
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




	char msg[100] = "0";
	send(clntSock, msg, strlen(msg), 0);


	*/

	int size = totalMap.size();

	map<pair<string, string>, double>::iterator it;
	//한번 출력해서 확인해본다.-------------------------------------------------------------------
	//이부분나중에 지울 것.

	ofstream otmp("../../../../keyword manager DB1/3-3-0.final_increase_TF/" + thisTime + "analyzeResult.txt");
	for (it = totalMap.begin(); it != totalMap.end(); ++it)
	{
		otmp << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
	}
	
	//--------------------------------------------------------------------------------------
	
	Local<Array> nn = Array::New(isolate, size);

	
	int i = 0;
	for (it = totalMap.begin(); it != totalMap.end(); ++it, ++i)
	{
		nn->Set(i, String::NewFromUtf8(isolate, (*it).first.first.c_str()));
	}
	

	Local<Array> nt = Array::New(isolate, size);
	i = 0;
	for (it = totalMap.begin(); it != totalMap.end(); ++it, ++i)
	{
		nt->Set(i, String::NewFromUtf8(isolate, (*it).first.second.c_str()));
	}
	

	Local<Array> ff = Array::New(isolate, size);
	i = 0;
	for (it = totalMap.begin(); it != totalMap.end(); ++it, ++i)
	{
		ff->Set(i, Number::New(isolate,(*it).second));
	}
	

	Local<Object> obj = Object::New(isolate);


	obj->Set(String::NewFromUtf8(isolate, "noun"), nn);
	obj->Set(String::NewFromUtf8(isolate, "nounType"), nt);
	obj->Set(String::NewFromUtf8(isolate, "frequency"), ff);

	args.GetReturnValue().Set(obj);

	/*double value = args[0]->NumberValue() + args[1]->NumberValue();
	Local<Number> num = Number::New(isolate, value);

	Local<String> str = String::NewFromUtf8(isolate, "success!");
	//std::string ddd = "IUZZANG";
	args.GetReturnValue().Set(str);
	//args.GetReturnValue().Set(num);*/

}

void Init(Handle<Object> exports, Handle<Object> module) {
	NODE_SET_METHOD(module, "exports", CreateObject);
}

NODE_MODULE(addon, Init)