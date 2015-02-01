﻿#include <iostream>
#include <fstream>
#include <mecab.h>
#include <string>
#include <Windows.h>
#include <cstdio>
#include <map>
#include <vector>
#include <algorithm>
#include <time.h>

#include "header.h"

using namespace std;


// Sample of MeCab::Tagger class.
int main(int argc, char **argv) {

	cout << "***  keyword analyzer(MECAB)  ***" << endl;

	while (1)
	{
		cout << "START?" ;
		string tmp;
		cin >> tmp;
		if (tmp == "n") break;


		time_t timer = time(NULL);
		tm* t = localtime(&timer);

		string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
			+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);

		string mecabInputTXT = "../../../../keyword manager DB/1.input/input.txt";
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
			if (ImecabInput.eof()) break;

			if (input == "") continue;

			// Gets tagged result in string format.
			const char *result = tagger->parse(input.c_str());
			CHECK(result);

			OmecabOutput << result << endl;

		}

		delete tagger;

		ImecabInput.close();
		OmecabOutput.close();


		//분석모듈
		ifstream ImecabOutput(mecabOutputTXT);
		ofstream OmecabAnalyze(mecabAnalyzeTXT);

		string apstr;
		bool continuous = false;
		int cnt = 0;
		while (1)
		{
			string line;
			getline(ImecabOutput, line, '\n');
			if (ImecabOutput.eof()) break;
			if (line.empty() || line[0] == '\t' || line == "EOS") continue;

			size_t pos = line.find('\t');
			string noun = line.substr(0, pos);

			if (noun == "﻿")
			{
				cout << "?" << line << endl;
			}

			size_t dotPos = line.find(',', pos + 1);
			string ty = line.substr(pos + 1, dotPos - pos - 1);

			if ((ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP" || ty == "SL") && continuous == true)
			{
				OmecabAnalyze << noun << '\t' << ty << endl;
				stringTypeNum[make_pair(noun, ty)]++;
				apstr.append(noun + " ");
				cnt++;
			}
			else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP" || ty == "SL")
			{
				OmecabAnalyze << noun << '\t' << ty << endl;
				stringTypeNum[make_pair(noun, ty)]++;
				continuous = true;
				apstr.append(noun + " ");
				cnt++;
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
		int maxx = 0;
		map<pair<string, string>, int>::iterator it;
		for (it = stringTypeNum.begin(); it != stringTypeNum.end(); ++it)
		{
			OmecabAnalyze << (*it).first.first << '\t' << (*it).first.second
				<< '\t' << (*it).second << endl;
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

		multimap<int, pair<string, string> >::iterator it2;
		for (it2 = numStringType.begin(); it2 != numStringType.end(); ++it2)
			OmecabAnalyze << (*it2).second.first << '\t' << (*it2).second.second << '\t' << (*it2).first << endl;


		ImecabOutput.close();
		OmecabAnalyze.close();

		ofstream OmecabTotalTF(mecabTotalTFTXT);

		for (it2 = numStringType.begin(); it2 != numStringType.end(); ++it2)
			OmecabTotalTF << (*it2).second.first << '\t' << (*it2).second.second << '\t' << (*it2).first << endl;

	}
	return 0;
}
