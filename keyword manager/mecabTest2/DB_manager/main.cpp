#include <iostream>
#include <Windows.h>
#include <map>
#include <string>
#include <fstream>
#include <math.h>

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

	//processing
	while (1)
	{
		cout << "CONTINUE?";
		string tmp;
		cin >> tmp;

		if (tmp == "n") break;
		documentN++;

		ifstream ImecabTotal(mecabTotalTFName);

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


	}
}