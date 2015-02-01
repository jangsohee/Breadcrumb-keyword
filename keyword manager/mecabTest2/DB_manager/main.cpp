#include <iostream>
#include <Windows.h>
#include <map>
#include <string>
#include <fstream>

using namespace std;

map<pair<string,string>, int> stringTypeNum;

int main()
{
	cout << "***  강석일 회원의 keyword DB를 쌓겠습니다..  ***" << endl;

	string mecabTotalTFName = "../../../../keyword manager DB/4.total_TF_input/00.total_TF_input.txt";
	string mecabTotalTFResult = "../../../../keyword manager DB/5.total_TF/00.total_TF_result.txt";

	//preprocess
	ifstream ImecabTotalTF(mecabTotalTFResult);

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

		if (tmp=="n") break;

		ifstream ImecabTotal (mecabTotalTFName);

		while (1)
		{
			string str;
			getline(ImecabTotal, str, '\t');
			if (ImecabTotal.eof()) break;

			string ty;
			ImecabTotal >> ty;

			int num;
			ImecabTotal >> num;
			
			stringTypeNum[make_pair(str,ty)] += num;

			//'\n' 제거
			getline(ImecabTotal, str, '\n');
		}

		ImecabTotal.close();
	}

	ofstream Oresult(mecabTotalTFResult.c_str());

	map<pair<string,string>, int>::iterator it;
	for (it = stringTypeNum.begin(); it != stringTypeNum.end(); ++it)
		Oresult << it->first.first << '\t' << it->first.second << '\t' << it->second << endl;

}