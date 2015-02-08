#include <iostream>
#include <WinSock2.h>
#include <fstream>
#include <mecab.h>
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

#include "header.h"

using namespace std;

#define SERVERIP "127.0.0.1"
#define PORT 74132


//**실험모듈을 수정할 것.**
//실제 노드에 얹혀서 서비스 하는 모델을 만들 것.
//while문 같이 실험하기 적합하게 한 구조는 수정을 해야겠지.
//일단 Increase IDF 모델하나만 던지게 끔 수정 할 것이다.
//파일에 읽고 쓰는 것을 최소화 할 것이다. (성능을 위해서 이것이 더 낫겠지?) 아니다 로그를 남겨야 하나. 트레이드 오프다. 음
//title, body 구분해서 분석할 것이다.
//모듈화 해서 두개로 형태소 분석, 연산 모듈 두개로 나누어야 할 것이다.
//소켓통신하는 부분도 제거 할 것.

//이렇게 많으니 꼬였지 젠장


// Sample of MeCab::Tagger class.
int main(int argc, char **argv) {

	/*string testStr = "hello my name is kang seok il nice to meet you";
	size_t found = 0,found2;

	while (1)
	{
	found2 = testStr.find(' ', found);
	cout << testStr.substr(found, found2-found) << endl;
	if (found2 == std::string::npos) break;
	found = found2 + 1;
	}

	return 0;*/

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


	//string mecabOutputTXT = "../../../../keyword manager DB1/2.mecab_output/" + thisTime + "output.txt";
	//ofstream OmecabOutput(mecabOutputTXT);


	// Gets tagged result in string format.
	const char *result = tagger->parse(title.c_str());
	//CHECK(result);
	string Atitle = result;
	//OmecabOutput << Atitle << endl<<endl;

	const char *result2 = tagger->parse(body.c_str());
	string Abody = result2;
	//OmecabOutput << Abody << endl;

	delete tagger;

	
	//OmecabOutput.close();


	//---------------------------------------분석모듈-------------------------------------------------

	


	//title 부터 분석.
	{
		string titleAnalyzeTXT = "../../../../keyword manager DB1/3-0-0.final_title/" + thisTime + "analyzeResult.txt";
		ofstream OtitleAnalyze(titleAnalyzeTXT);

		vector<string> vs;
		map<pair<string, string>, int> titleMap;
		//multimap<int, pair<string, string> > titleMultiMap;

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
		}


		//------------------------------------title 분석 완료---------------------------------------------
	}
	


	//body 분석.
	{
		string bodyAnalyzeTXT = "../../../../keyword manager DB1/3-0-1.final_body/" + thisTime + "analyzeResult.txt";
		ofstream ObodyAnalyze(bodyAnalyzeTXT);

		vector<string> vs;
		map<pair<string, string>, int> bodyMap;
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
		}


	}

	
	return 0;
	



//====================================================실험모듈==================================================


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


	cout << "START?";
	string tmp;
	cin >> tmp;
	if (tmp == "n") return 0;

	_finddata_t fd;
	long handle;
	int chkResult = 1;

	//handle = _findfirst("../../../../datamaker/realCrawling/DB/*.*", &fd);
	handle = _findfirst("../../../../keyword manager DB/1.input/*.*", &fd);

	if (handle == -1)
	{
		cout << "파일 읽기 오류" << endl;
		return 0;
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

		time_t timer = time(NULL);
		tm* t = localtime(&timer);

		string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
			+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);

		//string mecabInputTXT = "../../../../datamaker/realCrawling/DB/" + fdName;
		string mecabInputTXT = "../../../../keyword manager DB/1.input/" + fdName;

		string mecabOutputTXT = "../../../../keyword manager DB/2.mecab_output/" + thisTime + "output.txt";
		string mecabAnalyzeTXT = "../../../../keyword manager DB/3.final/" + thisTime + "analyzeResult.txt";
		string mecabTotalTFTXT = "../../../../keyword manager DB/4.total_TF_input/00.total_TF_input.txt";

		//mecab engine 구동
		MeCab::Tagger *tagger = MeCab::createTagger("-r C:/librarys/mecab-ko/mecabrc -d C:/librarys/mecab-ko/dic/mecab-ko-dic-1.6.1");
		CHECK(tagger);

		ifstream ImecabInput(mecabInputTXT);
		ofstream OmecabOutput(mecabOutputTXT);

		while (1)
		{
			string input;

			getline(ImecabInput, input, '\n');
			if (ImecabInput.eof())
			{
				const char *result = tagger->parse(input.c_str());
				CHECK(result);

				OmecabOutput << result << endl;
				break;
			}

			if (input == "") continue;

			// Gets tagged result in string format.
			const char *result = tagger->parse(input.c_str());
			CHECK(result);

			OmecabOutput << result << endl;
		}

		delete tagger;

		ImecabInput.close();
		OmecabOutput.close();


		//------------------------------------분석모듈-------------------------------------------------
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

		string mecabCountTXT = "../../../../keyword manager DB/3-0.simple_count_TF/00.countTFAnalyzeResult.txt";//  *
		string mecabBooleanTFTXT = "../../../../keyword manager DB/3-1.boolean_TF/00.booleanTFAnalyzeResult.txt";
		string mecabLogTXT = "../../../../keyword manager DB/3-2.log_TF/00.logTFAnalyzeResult.txt";
		string mecabIncreaseTXT = "../../../../keyword manager DB/3-3.increase_TF/00.increaseTFAnalyzeResult.txt";

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


		string mecabSelectivDF = "../../../../keyword manager DB/5-1.selectiveDF/00.selectve.txt";

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

		string simpleCountTFIDFResult = "../../../../keyword manager DB/7-0.simple_count_TF-IDF/" + thisTime + " SimpleCountTFIDFAnalyzeResult.txt";
		string BooleanTFIDFResult = "../../../../keyword manager DB/7-1.boolean_TF-IDF/" + thisTime + " BooleanTFIDFAnalyzeResult.txt";
		string LogTFIDFResult = "../../../../keyword manager DB/7-2.log_TF-IDF/" + thisTime + " LogTFIDFAnalyzeResult.txt";
		string IncreaseTFIDFResult = "../../../../keyword manager DB/7-3.increase_TF-IDF/" + thisTime + " IncreaseTFIDFAnalyzeResult.txt";

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

	return 0;
}
