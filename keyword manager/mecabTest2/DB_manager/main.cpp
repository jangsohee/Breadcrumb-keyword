#include <iostream>
#include <Windows.h>
#include <map>
#include <string>
#include <fstream>
#include <math.h>
#include <limits>

using namespace std;

map<pair<string,string>, double> stringTypeNum;

int main()
{
	cout << "***  강석일 회원의 keyword DB를 쌓겠습니다..  ***" << endl;

	string mecabTotalTFName = "../../../../keyword manager DB/4.total_TF_input/00.total_TF_input.txt";
	string mecabTotalTFResult = "../../../../keyword manager DB/5.total_TF/01.total_TF_result.txt";
	string mecabCalculateIDF = "../../../../keyword manager DB/6.caclulate_IDF/00.caculate.txt";
	string mecabBooleanTFTXT = "../../../../keyword manager DB/3-1.boolean_TF/00.booleanTFAnalyzeResult.txt";
	string mecabTFIDFResult = "../../../../keyword manager DB/7.TF-IDF/00.TFIDFAnalyzeResult.txt";

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

		ifstream ImecabBoolTXT(mecabBooleanTFTXT);
		ofstream OTFIDFResult(mecabTFIDFResult);
		
		multimap<double , pair<string, string> > IDFNounType;
		while (1)
		{
			string noun, ty;
			double booleanTF;
			getline(ImecabBoolTXT, noun, '\t');
			ImecabBoolTXT >> ty >> booleanTF;
			if (ImecabBoolTXT.eof()) break;

			double DF = stringTypeNum[make_pair(noun, ty)];
			double IDF = log10(documentN / DF);

			IDFNounType.insert(make_pair(IDF,make_pair(noun, ty)));
			
			string tmp;
			getline(ImecabBoolTXT, tmp, '\n');
		}
		
		multimap<double, pair<string, string> >::iterator it2;
		for (it2 = IDFNounType.begin(); it2 != IDFNounType.end(); ++it2)
		{
			OTFIDFResult << it2->second.first << '\t' << it2->second.second << '\t' << it2->first << endl;
		}

		ImecabBoolTXT.close();
		OTFIDFResult.close();

	}
}