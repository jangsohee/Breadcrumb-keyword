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

void funcTF(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	//실험 환경
	/*
	cout << "***  keyword analyzer(MECAB)  ***" << endl;
	cout << "start!" << endl;
	time_t timer = time(NULL);
	tm* t = localtime(&timer);
	string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
	+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);

	string mecabInputTXT = "../../../../keyword manager DB1/1.input/input.txt";
	ifstream ImecabInput(mecabInputTXT);

	string title;
	getline(ImecabInput, title, '\n');

	string body;
	getline(ImecabInput, body, '\0');

	ImecabInput.close();
	*/


	//서버 환경
	String::Utf8Value titleParam(args[0]->ToString());
	string title = *titleParam;
	String::Utf8Value bodyParam(args[1]->ToString());
	string body = *bodyParam;

	String::Utf8Value linkTextParam(args[2]->ToString());
	string linkText = *linkTextParam;
	String::Utf8Value parentKeywordParam(args[3]->ToString());
	string parentKeyword = *parentKeywordParam;

	//mecab engine 구동
	MeCab::Tagger *tagger = MeCab::createTagger("-r C:/librarys/mecab-ko/mecabrc -d C:/librarys/mecab-ko/dic/mecab-ko-dic-1.6.1");

	// Gets tagged result in string format.
	const char *result = tagger->parse(title.c_str());
	string Atitle = result;

	const char *result2 = tagger->parse(body.c_str());
	string Abody = result2;

	string ALinkText="";
	if (linkText != "")
	{
		const char *result3 = tagger->parse(linkText.c_str());
		ALinkText = result3;
	}
	


	//출력 확인
	/*
	string mecabOutputTXT = "../../../../keyword manager DB1/2.mecab_output/" + thisTime + "output.txt";
	ofstream OmecabOutput(mecabOutputTXT);
	OmecabOutput << Atitle << endl << endl;
	OmecabOutput << Abody << endl;
	OmecabOutput.close();
	*/

	delete tagger;

	map<pair<string, string>, double> totalMap;

	//---------------------------------------분석모듈-------------------------------------------------

	map<pair<string, string>, int> titleMap;
	//title 부터 분석.
	{
		//string titleAnalyzeTXT = "../../../../keyword manager DB1/3-0-0.final_title/" + thisTime + "analyzeResult.txt";
		//ofstream OtitleAnalyze(titleAnalyzeTXT);

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
					//OtitleAnalyze << endl;
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
					//OtitleAnalyze << endl;
					continuous = false;
				}
				break;
			}

			if (line.empty() || line[0] == '\t' || line == "EOS")
			{
				if (continuous == true && cnt != 1)
				{
					//OtitleAnalyze << endl;
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
					//OtitleAnalyze << endl;
					continuous = false;
				}
				continue;
			}

			size_t pos = line.find('\t');
			string noun = line.substr(0, pos);

			/*
			if (noun == "﻿")
			{
			cout << "?" << line << endl;
			}
			*/

			size_t dotPos = line.find(',', pos + 1);
			string ty = line.substr(pos + 1, dotPos - pos - 1);


			// 영어 단어의 비율 체크 해보자.
			// 총 단어도 같이 세어보자. tf모델의 변형 가능성 열어두기 위함.

			if ((ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP") && continuous == true)
			{
				//OtitleAnalyze << noun << '\t' << ty << endl;
				titleMap[make_pair(noun, ty)]++;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP")
			{
				//OtitleAnalyze << noun << '\t' << ty << endl;
				titleMap[make_pair(noun, ty)]++;
				continuous = true;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "SL" && continuous == true)
			{
				//OtitleAnalyze << noun << '\t' << ty << endl;
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
				//OtitleAnalyze << noun << '\t' << ty << endl;
				titleMap[make_pair(noun, ty)]++;
			}
			else if (continuous == true && cnt != 1)
			{
				//OtitleAnalyze << endl;
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
				//OtitleAnalyze << endl;
				continuous = false;
				cnt = 0;
				apstr.erase();
			}
			found = found2 + 1;
		}
		//OtitleAnalyze << "----------------------------------------------------------------------------------" << endl;

		string ans;
		double maxx = 0;
		map<pair<string, string>, int>::iterator it;
		for (it = titleMap.begin(); it != titleMap.end(); ++it)
		{
			//OtitleAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
			if (maxx < (*it).second)
			{
				maxx = (*it).second;
				ans = (*it).first.first;
			}
		}

		/*
		OtitleAnalyze << endl << "----------------------------------------------------------------------------------" << endl;
		OtitleAnalyze << "keyword : " << ans;
		OtitleAnalyze << endl << "----------------------------------------------------------------------------------";
		OtitleAnalyze << endl;
		*/

//TitleText에 대한 가중치 : 0.5 ~ 1
		for (it = titleMap.begin(); it != titleMap.end(); ++it)
		{
			double increaseTF = 0.5 + (0.5 * (*it).second) / maxx;
			//OtitleAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << increaseTF << endl;
			totalMap[make_pair((*it).first.first, (*it).first.second)] += increaseTF;
		}
	}

	map<pair<string, string>, int> bodyMap;
	//body 분석.
	{
		//string bodyAnalyzeTXT = "../../../../keyword manager DB1/3-0-1.final_body/" + thisTime + "analyzeResult.txt";
		//ofstream ObodyAnalyze(bodyAnalyzeTXT);
		vector<string> vs;
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
					//ObodyAnalyze << endl;
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
					//ObodyAnalyze << endl;
					continuous = false;
				}
				break;
			}

			if (line.empty() || line[0] == '\t' || line == "EOS")
			{
				if (continuous == true && cnt != 1)
				{
					//ObodyAnalyze << endl;
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
					//ObodyAnalyze << endl;
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
				//ObodyAnalyze << noun << '\t' << ty << endl;
				bodyMap[make_pair(noun, ty)]++;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP")
			{
				//ObodyAnalyze << noun << '\t' << ty << endl;
				bodyMap[make_pair(noun, ty)]++;
				continuous = true;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "SL" && continuous == true)
			{
				//ObodyAnalyze << noun << '\t' << ty << endl;
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
				//ObodyAnalyze << noun << '\t' << ty << endl;
				bodyMap[make_pair(noun, ty)]++;
			}
			else if (continuous == true && cnt != 1)
			{
				//ObodyAnalyze << endl;
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
				//ObodyAnalyze << endl;
				continuous = false;
				cnt = 0;
				apstr.erase();
			}


			found = found2 + 1;
		}
		//ObodyAnalyze << "----------------------------------------------------------------------------------" << endl;

		string ans;
		double maxx = 0;
		map<pair<string, string>, int>::iterator it;
		for (it = bodyMap.begin(); it != bodyMap.end(); ++it)
		{
			//ObodyAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
			if (maxx < (*it).second)
			{
				maxx = (*it).second;
				ans = (*it).first.first;
			}
		}

		/*
		ObodyAnalyze << endl << "----------------------------------------------------------------------------------" << endl;
		ObodyAnalyze << "keyword : " << ans;
		ObodyAnalyze << endl << "----------------------------------------------------------------------------------";
		ObodyAnalyze << endl;
		*/

//body에 대한 가중치 : 0.5 ~ 1
		for (it = bodyMap.begin(); it != bodyMap.end(); ++it)
		{
			double increaseTF = 0.5 + (0.5 * (*it).second) / maxx;
			//ObodyAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << increaseTF << endl;
			totalMap[make_pair((*it).first.first, (*it).first.second)] += increaseTF;
		}
	}

	map<pair<string, string>, int> linkMap;
	//linkText 분석.
	if (ALinkText != "")
	{
		//string bodyAnalyzeTXT = "../../../../keyword manager DB1/3-0-1.final_body/" + thisTime + "analyzeResult.txt";
		//ofstream ObodyAnalyze(bodyAnalyzeTXT);
		vector<string> vs;
		string apstr;
		bool continuous = false;
		int cnt = 0;
		size_t found = 0, found2;

		while (1)
		{
			found2 = ALinkText.find('\n', found);
			string line = ALinkText.substr(found, found2 - found);

			if (found2 == std::string::npos || line == "EOS")
			{
				if (continuous == true && cnt != 1)
				{
					//ObodyAnalyze << endl;
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
							linkMap[make_pair(appendStr, "MyCompound")]++;
						}
					}
					vs.clear();
					cnt = 0;
				}
				else if (continuous == true && cnt == 1)
				{
					apstr.clear();
					cnt = 0;
					//ObodyAnalyze << endl;
					continuous = false;
				}
				break;
			}

			if (line.empty() || line[0] == '\t' || line == "EOS")
			{
				if (continuous == true && cnt != 1)
				{
					//ObodyAnalyze << endl;
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
							linkMap[make_pair(appendStr, "MyCompound")]++;
						}
					}
					vs.clear();
					cnt = 0;
				}
				else if (continuous == true && cnt == 1)
				{
					apstr.clear();
					cnt = 0;
					//ObodyAnalyze << endl;
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
				//ObodyAnalyze << noun << '\t' << ty << endl;
				linkMap[make_pair(noun, ty)]++;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP")
			{
				//ObodyAnalyze << noun << '\t' << ty << endl;
				linkMap[make_pair(noun, ty)]++;
				continuous = true;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "SL" && continuous == true)
			{
				//ObodyAnalyze << noun << '\t' << ty << endl;
				linkMap[make_pair(noun, ty)]++;
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
						linkMap[make_pair(appendStr, "MyCompound")]++;
					}
				}
				vs.clear();
				cnt = 0;

			}
			else if (ty == "SL")
			{
				//ObodyAnalyze << noun << '\t' << ty << endl;
				linkMap[make_pair(noun, ty)]++;
			}
			else if (continuous == true && cnt != 1)
			{
				//ObodyAnalyze << endl;
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
						linkMap[make_pair(appendStr, "MyCompound")]++;
					}
				}
				vs.clear();
				cnt = 0;
			}
			else
			{
				//ObodyAnalyze << endl;
				continuous = false;
				cnt = 0;
				apstr.erase();
			}


			found = found2 + 1;
		}
		//ObodyAnalyze << "----------------------------------------------------------------------------------" << endl;

		string ans;
		double maxx = 0;
		map<pair<string, string>, int>::iterator it;
		for (it = linkMap.begin(); it != linkMap.end(); ++it)
		{
			//ObodyAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
			if (maxx < (*it).second)
			{
				maxx = (*it).second;
				ans = (*it).first.first;
			}
		}

		/*
		ObodyAnalyze << endl << "----------------------------------------------------------------------------------" << endl;
		ObodyAnalyze << "keyword : " << ans;
		ObodyAnalyze << endl << "----------------------------------------------------------------------------------";
		ObodyAnalyze << endl;
		*/


//linkText에 대한 가중치 : 0~0.5
		for (it = linkMap.begin(); it != linkMap.end(); ++it)
		{
			double increaseTF = 0 + (0.5 * (*it).second) / maxx;
			//ObodyAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << increaseTF << endl;
			totalMap[make_pair((*it).first.first, (*it).first.second)] += increaseTF;
		}
	}


	int size = totalMap.size();

	map<pair<string, string>, double>::iterator it;

	//출력확인
	/*
	ofstream otmp("../../../../keyword manager DB1/3-3-0.final_increase_TF/00.analyzeResult.txt");
	for (it = totalMap.begin(); it != totalMap.end(); ++it)
	{
	otmp << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
	}
	*/

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
		ff->Set(i, Number::New(isolate, (*it).second));
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "noun"), nn);
	obj->Set(String::NewFromUtf8(isolate, "nounType"), nt);
	obj->Set(String::NewFromUtf8(isolate, "TF"), ff);
	obj->Set(String::NewFromUtf8(isolate, "ParentKeyword"), String::NewFromUtf8(isolate, parentKeyword.c_str()));

	args.GetReturnValue().Set(obj);
}

void funcExtract(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	
//실험환경
	/*
	cout << "calculate!" << endl;

	time_t timer = time(NULL);
	tm* t = localtime(&timer);
	string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
	+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);
	string mecabInputTXT = "../../../../keyword manager DB1/5-1-0.selectiveDF_TF/00.selectve.txt";
	ifstream ImecabInput(mecabInputTXT);
	int documentN;
	ImecabInput >> documentN;
	//해당 명사와 명사태그, TF를 받아오게끔
	string ttmp;
	//getline(ImecabInput, ttmp, '\n');
	while (1)
	{
	string noun, ty;
	int DF;
	double iTF;

	//getline(ImecabInput, noun, '\t');

	if (ImecabInput.eof()) break;
	ImecabInput >> ty >> DF >> iTF;

	//해당 단어의 IDF값을 구하고,

	double IDF = log10(documentN / DF);

	// 상황에 맞는 TF와 곱한다.
	double iTFIDF = iTF * IDF;

	iTFIDFNounType.insert(make_pair(iTFIDF, make_pair(noun, ty)));

	string ttmp;

	getline(ImecabInput, ttmp, '\n');
	}
	*/

//서버 환경
	multimap<double, pair<string, string> > TFIDFNounType;

	int documentN = args[0]->Int32Value();
	Local<Array> Anoun = Local<Array>::Cast(args[1]);
	Local<Array> AnounType = Local<Array>::Cast(args[2]);
	Local<Array> ADF = Local<Array>::Cast(args[3]);
	Local<Array> ATF = Local<Array>::Cast(args[4]);
	String::Utf8Value parentKeywordParam(args[5]->ToString());
	string parentKeyword = *parentKeywordParam;

	int arSize = Anoun->Length();

	for (int i = 0; i < arSize; ++i)
	{
		//noun 받기
		String::Utf8Value nounPara(Anoun->Get(Integer::New(isolate, i)->ToString()));
		string noun = *nounPara;

		//nounType 받기
		String::Utf8Value nounTypePara(AnounType->Get(Integer::New(isolate, i)->ToString()));
		string nounType = *nounTypePara;

		//freq 받기
		int DF = ADF->Get(Integer::New(isolate, i))->Int32Value();

		//TF 받기
		double TF = ATF->Get(Integer::New(isolate, i))->NumberValue();

		double IDF = log10(documentN / DF);

		double TFIDF = TF*IDF;

		TFIDFNounType.insert(make_pair(TFIDF, make_pair(noun, nounType)));
	}

	/* 출력 확인
	string IncreaseTFIDFResult = "../../../../keyword manager DB1/7-3.increase_TF-IDF/" + thisTime + " IncreaseTFIDFAnalyzeResult.txt";
	ofstream OIncreaseTFIDF(IncreaseTFIDFResult);
	multimap<double, pair<string, string> >::reverse_iterator it3;
	for (it3 = TFIDFNounType.rbegin(); it3 != TFIDFNounType.rend(); ++it3)
	{
	OIncreaseTFIDF << it3->second.first << '\t' << it3->second.second << '\t' << it3->first << endl;
	}
	*/

	Local<Array> keyword = Array::New(isolate, 1);
	multimap<double, pair<string, string> >::reverse_iterator it = TFIDFNounType.rbegin();
	
	//만약 부모키워드와 같을 시
	if (parentKeyword!="" && it->second.first == parentKeyword)
		it++;
	
	keyword->Set(0, String::NewFromUtf8(isolate, it->second.first.c_str()));
	
	args.GetReturnValue().Set(keyword);
}


void Init(Handle<Object> exports) {
	NODE_SET_METHOD(exports, "funcTF", funcTF);
	NODE_SET_METHOD(exports, "funcExtract", funcExtract);
}

NODE_MODULE(addon, Init)