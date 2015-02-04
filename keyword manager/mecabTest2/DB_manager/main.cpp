#include <iostream>
#include <Windows.h>
#include <map>
#include <string>
#include <fstream>
#include <math.h>
#include <limits>
#include <time.h>

using namespace std;

map<pair<string,string>, double> stringTypeNum;

int main()
{
	cout << "***  강석일 회원의 keyword DB를 쌓겠습니다..  ***" << endl;

	string mecabTotalTFName = "../../../../keyword manager DB/4.total_TF_input/00.total_TF_input.txt";
	string mecabTotalTFResult = "../../../../keyword manager DB/5.total_TF/01.total_TF_result.txt";
	string mecabCalculateIDF = "../../../../keyword manager DB/6.caclulate_IDF/00.caculate.txt";
	
	//preprocess
	ifstream ImecabTotalTF(mecabTotalTFResult);

	string ttmp;
	getline(ImecabTotalTF, ttmp, '\n');
	//getline(ImecabTotalTF, ttmp, '\n');
	//documentN = atoi(ttmp.c_str());

	double documentN;

	ImecabTotalTF >> documentN;

	getline(ImecabTotalTF, ttmp, '\n');

	while(1)
	{
		string str;
		getline(ImecabTotalTF, str, '\t');
		if (ImecabTotalTF.eof()) break;

		string ty;
		ImecabTotalTF >> ty;

		int num;
		ImecabTotalTF >> num;

		stringTypeNum[make_pair(str, ty)] += num;

		//'\n' 제거
		getline(ImecabTotalTF, str, '\n');
	}

	ImecabTotalTF.close();

	
	while (1)
	{
		cout << "CONTINUE?";
		string tmp;
		cin >> tmp;

		if (tmp == "n") break;
		documentN++;

		ifstream ImecabTotal(mecabTotalTFName);

		//DF값 추가.
		while (1)
		{
			string str;
			getline(ImecabTotal, str, '\t');
			if (ImecabTotal.eof()) break;

			string ty;
			ImecabTotal >> ty;

			int num;
			ImecabTotal >> num;

			stringTypeNum[make_pair(str, ty)] += num;

			//'\n' 제거
			getline(ImecabTotal, str, '\n');
		}

		ImecabTotal.close();
		//}


		//추가된 DF를 기록.
		ofstream Oresult(mecabTotalTFResult);

		Oresult << endl << documentN << endl;
		map<pair<string, string>, double>::iterator it;
		for (it = stringTypeNum.begin(); it != stringTypeNum.end(); ++it)
			Oresult << it->first.first << '\t' << it->first.second << '\t' << it->second << endl;

		ofstream OCaculateIDF(mecabCalculateIDF);
		for (it = stringTypeNum.begin(); it != stringTypeNum.end(); ++it)
		{
			double IDF = documentN / it->second;
			OCaculateIDF << it->first.first << '\t' << it->first.second << '\t' << log10(IDF) << endl;
		}

		Oresult.close();
		OCaculateIDF.close();

		//IDF calculate

		string mecabSimpleCountTFTXT = "../../../../keyword manager DB/3-0.simple_count_TF/00.countTFAnalyzeResult.txt";
		string mecabBooleanTFTXT = "../../../../keyword manager DB/3-1.boolean_TF/00.booleanTFAnalyzeResult.txt";
		string mecabLogTFTXT = "../../../../keyword manager DB/3-2.log_TF/00.logTFAnalyzeResult.txt";
		string mecabIncreaseTFTXT = "../../../../keyword manager DB/3-3.increase_TF/00.increaseTFAnalyzeResult.txt";

		time_t timer = time(NULL);
		tm* t = localtime(&timer);

		string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
			+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);

		string simpleCountTFIDFResult = "../../../../keyword manager DB/7-0.simple_count_TF-IDF/"+thisTime+" SimpleCountTFIDFAnalyzeResult.txt";
		string BooleanTFIDFResult = "../../../../keyword manager DB/7-1.boolean_TF-IDF/" + thisTime + " BooleanTFIDFAnalyzeResult.txt";
		string LogTFIDFResult = "../../../../keyword manager DB/7-2.log_TF-IDF/" + thisTime + " LogTFIDFAnalyzeResult.txt";
		string IncreaseTFIDFResult = "../../../../keyword manager DB/7-3.increase_TF-IDF/" + thisTime + " IncreaseTFIDFAnalyzeResult.txt";

		ifstream ImecabSimpleCountTXT(mecabSimpleCountTFTXT);
		ifstream ImecabBoolTXT(mecabBooleanTFTXT);
		ifstream ImecabLogTXT(mecabLogTFTXT);
		ifstream ImecabIncreaseTXT(mecabIncreaseTFTXT);
		
		multimap<double , pair<string, string> > cTFIDFNounType;
		multimap<double, pair<string, string> > bTFIDFNounType;
		multimap<double, pair<string, string> > lTFIDFNounType;
		multimap<double, pair<string, string> > iTFIDFNounType;

		while (1)
		{
			string noun, ty;
			double countTF, booleanTF, LogTF, IncreaseTF;

			getline(ImecabSimpleCountTXT, noun, '\t');
			ImecabSimpleCountTXT >> ty >> countTF;
			
			getline(ImecabBoolTXT, noun, '\t');
			ImecabBoolTXT >> ty >> booleanTF;

			getline(ImecabLogTXT, noun, '\t');
			ImecabLogTXT >> ty >> LogTF;
			
			getline(ImecabIncreaseTXT, noun, '\t');
			ImecabIncreaseTXT >> ty >> IncreaseTF;
			if (ImecabIncreaseTXT.eof()) break;
	

			//해당 단어의 IDF값을 구하고,
			double DF = stringTypeNum[make_pair(noun, ty)];
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
			getline(ImecabSimpleCountTXT, ttmp, '\n');
			getline(ImecabBoolTXT, ttmp, '\n');
			getline(ImecabLogTXT, ttmp, '\n');
			getline(ImecabIncreaseTXT, ttmp, '\n');
		}
		
		ofstream OCountTFIDF(simpleCountTFIDFResult);
		ofstream OBooleanTFIDF(BooleanTFIDFResult);
		ofstream OLogTFIDF(LogTFIDFResult);
		ofstream OIncreaseTFIDF(IncreaseTFIDFResult);

		multimap<double, pair<string, string> >::reverse_iterator it2;
		for (it2 = cTFIDFNounType.rbegin(); it2 != cTFIDFNounType.rend(); ++it2)
		{
			OCountTFIDF << it2->second.first << '\t' << it2->second.second << '\t' << it2->first << endl;
		}

		for (it2 = bTFIDFNounType.rbegin(); it2 != bTFIDFNounType.rend(); ++it2)
		{
			OBooleanTFIDF << it2->second.first << '\t' << it2->second.second << '\t' << it2->first << endl;
		}

		for (it2 = lTFIDFNounType.rbegin(); it2 != lTFIDFNounType.rend(); ++it2)
		{
			OLogTFIDF << it2->second.first << '\t' << it2->second.second << '\t' << it2->first << endl;
		}

		for (it2 = iTFIDFNounType.rbegin(); it2 != iTFIDFNounType.rend(); ++it2)
		{
			OIncreaseTFIDF << it2->second.first << '\t' << it2->second.second << '\t' << it2->first << endl;
		}

		ImecabBoolTXT.close();
		OCountTFIDF.close();
		OBooleanTFIDF.close();
		OLogTFIDF.close();
		OIncreaseTFIDF.close();
	}
}