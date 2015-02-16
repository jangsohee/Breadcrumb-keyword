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
#include <random>

#include "header.h"

using namespace std;

#define SERVERIP "127.0.0.1"
#define PORT 74132


// Sample of MeCab::Tagger class.
int main(int argc, char **argv) {

	

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

	handle = _findfirst("../../../../datamaker/realCrawling/DB/*.*", &fd);
	//handle = _findfirst("../../../../keyword manager DB/1.input/*.*", &fd);

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
		
		srand(time(NULL));
		time_t timer = time(NULL);
		tm* t = localtime(&timer);
		SYSTEMTIME cur_time;
		GetLocalTime(&cur_time);
		string mscnds = to_string(cur_time.wMilliseconds);
		if (mscnds.length() == 2) mscnds += "0";
		string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
			+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + mscnds;
		
		

		//string mecabAnalyzeTXT = "../../../../keyword manager DB/3.final/" + thisTime + "analyzeResult.txt";
		//string mecabTotalTFTXT = "../../../../keyword manager DB/4.total_TF_input/00.total_TF_input.txt";

		//mecab engine 구동
		MeCab::Tagger *tagger = MeCab::createTagger("-r C:/librarys/mecab-ko/mecabrc -d C:/librarys/mecab-ko/dic/mecab-ko-dic-1.6.1");
		CHECK(tagger);

		ifstream ImecabInput("../../../../datamaker/realCrawling/DB/" + fdName/*string mecabInputTXT = "../../../../keyword manager DB/1.input/" + fdName*/);
		ofstream OmecabOutput("../../../../keyword manager DB/2.mecab_output/" + thisTime + "output.txt");

		string title;
		getline(ImecabInput, title, '\n');

		string body;
		getline(ImecabInput, body, '\0');

		const char *result = tagger->parse(title.c_str());
		string Atitle = result;

		const char *result2 = tagger->parse(body.c_str());
		string Abody = result2;

		OmecabOutput << Atitle << endl << Abody << endl;

		delete tagger;

		map<pair<string, string>, double> totalMap;
		ofstream OFAnlayze("../../../../keyword manager DB/3.final/" + thisTime + "analyzeResult.txt");

		//---------------------------------------분석모듈-------------------------------------------------

		map<pair<string, string>, int> titleMap;
		//title 부터 분석.
		{
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
						OFAnlayze << endl;
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
						OFAnlayze << endl;
						continuous = false;
					}
					break;
				}

				if (line.empty() || line[0] == '\t' || line == "EOS")
				{
					if (continuous == true && cnt != 1)
					{
						OFAnlayze << endl;
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
						OFAnlayze << endl;
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

				if ((ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP" || ty == "VV+ETN") && continuous == true)
				{
					OFAnlayze << noun << '\t' << ty << endl;
					titleMap[make_pair(noun, ty)]++;
					apstr.append(noun + " ");
					cnt++;
				}
				else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP" || ty == "VV+ETN")
				{
					OFAnlayze << noun << '\t' << ty << endl;
					titleMap[make_pair(noun, ty)]++;
					continuous = true;
					apstr.append(noun + " ");
					cnt++;
				}
				else if (ty == "SL" && continuous == true)
				{
					OFAnlayze << noun << '\t' << ty << endl;
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
					OFAnlayze << noun << '\t' << ty << endl;
					titleMap[make_pair(noun, ty)]++;
				}
				else if (continuous == true && cnt != 1)
				{
					OFAnlayze << endl;
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
					OFAnlayze << endl;
					continuous = false;
					cnt = 0;
					apstr.erase();
				}
				found = found2 + 1;
			}

			OFAnlayze << endl;
			OFAnlayze << "====================================count====================================" << endl;
			string ans;
			double maxx = 0;
			
			multimap<double, pair<string, string> > mm;
			map<pair<string, string>, int>::iterator it;
			for (it = titleMap.begin(); it != titleMap.end(); ++it)
			{
				OFAnlayze << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
				if (maxx < (*it).second)
				{
					maxx = (*it).second;
					ans = (*it).first.first;
				}
				mm.insert(make_pair((*it).second, make_pair((*it).first.first, (*it).first.second)));
			}
			OFAnlayze << "===================================================================================" << endl<<endl;

//타이틀 가중치
			OFAnlayze << "====================================ascend sort====================================" << endl;
			multimap<double, pair<string, string> >::iterator it2;
			for (it2 = mm.begin(); it2 != mm.end(); ++it2)
			{
				double increaseTF = 0 + (0.5 * (*it2).first) / maxx;
				totalMap[make_pair((*it2).second.first, (*it2).second.second)] += increaseTF;
				OFAnlayze << (*it2).second.first << '\t' << (*it2).second.second << '\t' << increaseTF << endl;
			}
			OFAnlayze << "==================================================================================" << endl<<endl;

			OFAnlayze << "==================================================================================" << endl;
			OFAnlayze << "title freq top : " << ans << endl;
			OFAnlayze << "==================================================================================" << endl<<endl;	


			/*
			for (it = titleMap.begin(); it != titleMap.end(); ++it)
			{
				double increaseTF = 0.5 + (0.5 * (*it).second) / maxx;
				OFAnlayze << (*it).first.first << '\t' << (*it).first.second << '\t' << increaseTF << endl;
				totalMap[make_pair((*it).first.first, (*it).first.second)] += increaseTF;
			}
			*/
		}

		map<pair<string, string>, int> bodyMap;
		//***
		map<pair<string, string>, vector<int> > locMap;
		map<pair<string, string>, pair<double, double> > mga;
		double maxMga = -9;
		double maxMgga = 0;
		//***
		int loc = 0;

		//body 분석.
		{
			vector<string> vs;
			string apstr;
			bool continuous = false;
			int cnt = 0;
			size_t found = 0, found2=0;
			
			while (1)
			{
				
				found2 = Abody.find('\n', found);
				string line = Abody.substr(found, found2 - found);

				if (found2 == std::string::npos || line == "EOS")
				{
					if (continuous == true && cnt != 1)
					{
						OFAnlayze << endl;
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

		//***
								locMap[make_pair(appendStr, "MyCompound")].push_back(loc - cnt + k);
								
		//***

							}
						}
						vs.clear();
						cnt = 0;
					}
					else if (continuous == true && cnt == 1)
					{
						apstr.clear();
						cnt = 0;
						OFAnlayze << endl;
						continuous = false;
					}
					break;
				}

				if (line.empty() || line[0] == '\t' || line == "EOS")
				{
					if (continuous == true && cnt != 1)
					{
						OFAnlayze << endl;
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

		//***
								locMap[make_pair(appendStr, "MyCompound")].push_back(loc - cnt + k);
		//***
							}
						}
						vs.clear();
						cnt = 0;
					}
					else if (continuous == true && cnt == 1)
					{
						apstr.clear();
						cnt = 0;
						OFAnlayze << endl;
						continuous = false;
					}
					continue;
				}

				size_t pos = line.find('\t');
				string noun = line.substr(0, pos);
				
				//***
//***여길 보시오	

				//***


				if (noun == "﻿")
				{
					cout << "?" << line << endl;
				}

				size_t dotPos = line.find(',', pos + 1);
				string ty = line.substr(pos + 1, dotPos - pos - 1);


				// 영어 단어의 비율 체크 해보자.
				// 총 단어도 같이 세어보자. tf모델의 변형 가능성 열어두기 위함.

	//****************************************추가
				if ((ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP" || ty == "VV+ETN") && cnt > 5)
				{
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

		//***
							locMap[make_pair(appendStr, "MyCompound")].push_back(loc - cnt + k);
		//***
						}
					}
					vs.clear();
					cnt = 0;

					OFAnlayze << noun << '\t' << ty << endl;
					bodyMap[make_pair(noun, ty)]++;
		//*****
					locMap[make_pair(noun, ty)].push_back(loc);
		//*****
					apstr.append(noun + " ");
					cnt++;
				}
	//8**************************************추가

				else if ((ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP" || ty == "VV+ETN") && continuous == true)
				{
					OFAnlayze << noun << '\t' << ty << endl;
					bodyMap[make_pair(noun, ty)]++;
	//*****
					locMap[make_pair(noun, ty)].push_back(loc);
	//*****
					apstr.append(noun + " ");
					cnt++;
				}
				else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP" || ty == "VV+ETN")
				{
					OFAnlayze << noun << '\t' << ty << endl;
					bodyMap[make_pair(noun, ty)]++;

	//*****
					locMap[make_pair(noun, ty)].push_back(loc);
	//*****
					continuous = true;
					apstr.append(noun + " ");
					cnt++;
				}
				else if (ty == "SL" && continuous == true)
				{
					OFAnlayze << noun << '\t' << ty << endl;
					bodyMap[make_pair(noun, ty)]++;

	//*****
					locMap[make_pair(noun, ty)].push_back(loc);
	//*****
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
		//***
							locMap[make_pair(appendStr, "MyCompound")].push_back(loc - cnt + k);
		//***
						}
					}
					vs.clear();
					cnt = 0;

				}
				else if (ty == "SL")
				{
					OFAnlayze << noun << '\t' << ty << endl;
					bodyMap[make_pair(noun, ty)]++;
		//*****
					locMap[make_pair(noun, ty)].push_back(loc);
		//*****
				}
				else if (continuous == true && cnt != 1)
				{
					OFAnlayze << endl;
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

		//***
							locMap[make_pair(appendStr, "MyCompound")].push_back(loc - cnt + k);
		//***
						}
					}
					vs.clear();
					cnt = 0;
				}
				else
				{
					OFAnlayze << endl;
					continuous = false;
					cnt = 0;
					apstr.erase();
				}
				found = found2 + 1;
				loc++;
			}
			OFAnlayze << endl;
			OFAnlayze << "====================================count====================================" << endl;
			string ans;
			double maxx = 0;

			multimap<double, pair<string, string> > mm;
			map<pair<string, string>, int>::iterator it;
			for (it = bodyMap.begin(); it != bodyMap.end(); ++it)
			{
				OFAnlayze << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
				if (maxx < (*it).second)
				{
					maxx = (*it).second;
					ans = (*it).first.first;
				}
				mm.insert(make_pair((*it).second, make_pair((*it).first.first, (*it).first.second)));
			}
			OFAnlayze << "===================================================================================" << endl<<endl;

			OFAnlayze << "====================================ascend sort====================================" << endl;

//바디 가중치
			multimap<double, pair<string, string> >::iterator it2;
			for (it2 = mm.begin(); it2 != mm.end(); ++it2)
			{
				double increaseTF = 0 + ( 3.5 * (*it2).first) / maxx;
				totalMap[make_pair((*it2).second.first, (*it2).second.second)] += increaseTF;
				OFAnlayze << (*it2).second.first << '\t' << (*it2).second.second << '\t' << increaseTF << endl;
			}
			OFAnlayze << "===================================================================================" << endl;
			OFAnlayze << "body freq top : " << ans << endl;
			OFAnlayze << "===================================================================================" << endl<<endl;

//*******출력 확인
			map<pair<string, string>, vector<int> >::iterator itloc;
			vector<pair<string, string> >sPair;
			for (itloc = locMap.begin(); itloc != locMap.end(); ++itloc)
			{
				if (itloc->second.size() == 1 || itloc->second.size() == 2)
				{
					sPair.push_back(make_pair(itloc->first.first, itloc->first.second));
					continue;
				}
				sort(itloc->second.begin(), itloc->second.end());
			}

			
			
			ofstream oloc("../../../../keyword manager DB/8.loc/" + thisTime + "analyzeResult.txt");
			for (itloc = locMap.begin(); itloc != locMap.end(); ++itloc)
			{
				oloc << itloc->first.first << '\t' << itloc->first.second << '\t';
				for (int i = 0; i < itloc->second.size()-1; ++i)
				{
					oloc << itloc->second[i]<<'\t';
				}
				oloc << itloc->second[itloc->second.size() - 1]<<endl;
			}

			for (int i = 0; i < sPair.size(); ++i)
			{
				locMap.erase(make_pair(sPair[i].first, sPair[i].second));
			}

			oloc << "============================================================" << endl << endl << endl;

			
			for (itloc = locMap.begin(); itloc != locMap.end(); ++itloc)
			{
				vector<int> vi = itloc->second;
				double len = vi[vi.size() - 1] - vi[0];
				double gap = len / (vi.size() - 1);

				//퍼져있는가
				double ga = log10((len / loc + 1));

				double cent = vi[0] + (len / 2);

//분포도 좁게 있을 수록 높은 값
				double ga2 = 0; //역수 취해야
				//double tmpGap = gap;
			/*	for (int i = 1; i <= vi.size()-1; ++i)
				{
					ga2 += (tmpGap - vi[i])*(tmpGap - vi[i]);
					tmpGap += gap;
				}
				*/
				for (int i = 0; i < vi.size(); ++i)
				{
					ga2 += (cent - vi[i])*(cent - vi[i]);
				}

				//ga2 /= (vi.size() - 1);
				ga2 = sqrt(ga2);
				//ga2 /= len;
				//ga2 /= (vi.size() - 1);
				double iga2 = 1 / ga2;

				oloc << itloc->first.first << '\t' << itloc->first.second << '\t' << ga << '\t' << iga2 << endl;
				mga.insert(make_pair(make_pair(itloc->first.first, itloc->first.second), make_pair(ga, iga2)));
				if (maxMga < iga2) maxMga = iga2;
			}
//****		
			oloc << "======================================jenguhwa=============================" << endl;
			map<pair<string, string>, pair<double,double> >::iterator mgaIt;
			
			for (mgaIt = mga.begin(); mgaIt != mga.end(); ++mgaIt)
			{
				double g1 = mgaIt->second.first;
				double g2 = mgaIt->second.second;
				
				double gg2 = 0.1 + (g2 / maxMga) * 0.1;
				if (maxMgga < gg2*g1) maxMgga = gg2*g1*100;


				oloc << mgaIt->first.first << '\t' << mgaIt->first.second << '\t' << g1 << '\t' << gg2 << '\t' << g1 * gg2 << endl;
			}
//********

			/*
			for (it = bodyMap.begin(); it != bodyMap.end(); ++it)
			{
				double increaseTF = 0.5 + (0.5 * (*it).second) / maxx;
				//ObodyAnalyze << (*it).first.first << '\t' << (*it).first.second << '\t' << increaseTF << endl;
				totalMap[make_pair((*it).first.first, (*it).first.second)] += increaseTF;
			}
			*/
		}

		int size = totalMap.size();

		map<pair<string, string>, double>::iterator it;

		//출력확인
		ofstream otmp("../../../../keyword manager DB/4.total_TF_input/00.total_TF_input.txt");
		for (it = totalMap.begin(); it != totalMap.end(); ++it)
		{
			otmp << (*it).first.first << '\t' << (*it).first.second << '\t' << (*it).second << endl;
		}
		
		
		//==================================================================================
		char msg[100] = "1";
		send(clntSock, msg, strlen(msg), 0);

		int len = recv(clntSock, msg, 99, 0);
		msg[len] = 0;

		cout << "step2" << endl;
		//IDF calculate

		multimap<double, pair<string, string> > iTFIDFNounType;
		ifstream ISelectiveTDF("../../../../keyword manager DB/5-1.selectiveDF/00.selectve.txt");
		ofstream OCIDF("../../../../keyword manager DB/6.caclulate_IDF/00.selectve.txt");

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

			double IncreaseTF;
			IncreaseTF = totalMap[make_pair(noun, ty)];


			//해당 단어의 IDF값을 구하고,
			double IDF = log10(documentN / DF);
//******
			if (mga.find(make_pair(noun, ty)) != mga.end())
			{
				double g1 = mga[make_pair(noun, ty)].first;
				double g2 = mga[make_pair(noun, ty)].second;
				
				double gg2 = 0.1 + (g2 / maxMga) * 0.1;

				double fg = gg2*g1 * 10;
				IDF += (fg);
			}
//******
			OCIDF << noun << '\t' << ty << '\t' << IDF << endl;

			// 상황에 맞는 TF와 곱한다.
			double iTFIDF = IncreaseTF * IDF;

			iTFIDFNounType.insert(make_pair(iTFIDF, make_pair(noun, ty)));

			string ttmp;
			getline(ISelectiveTDF, ttmp, '\n');
		}

		ofstream OIncreaseTFIDF("../../../../keyword manager DB/7.TF-IDF/" + thisTime + " IncreaseTFIDFAnalyzeResult.txt");
		

		string cantKeyword[2] = { "Mask", "slide" };


		multimap<double, pair<string, string> >::reverse_iterator it3;
		for (it3 = iTFIDFNounType.rbegin(); it3 != iTFIDFNounType.rend(); ++it3)
		{
			if (it3->second.first == "Mask" || it3->second.first == "slide") continue;
			OIncreaseTFIDF << it3->second.first << '\t' << it3->second.second << '\t' << it3->first << endl;
		}
		OIncreaseTFIDF.close();
		chkResult = _findnext(handle, &fd);
	}

	char msg[100] = "0";
	send(clntSock, msg, strlen(msg), 0);

	
	return 0;
}
