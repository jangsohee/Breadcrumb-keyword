#include <iostream>
#include <Windows.h>
#include <map>
#include <string>
#include <fstream>

using namespace std;

map<string, int> mm;

int main()
{
	string mecabTotalTFName = "../../../../keyword manager DB/4.total_TF_input/00total_TF_input.txt";
	string mecabTotalTFResult = "../../../../keyword manager DB/5.total_TF/00.total_TF_result.txt";

	//preprocess
	ifstream mecabTatalTF(mecabTotalTFResult.c_str());

	while(1)
	{
		string str;
		getline(mecabTatalTF, str, '\t');
		if (mecabTatalTF.eof()) break;

		int num;
		mecabTatalTF >> num;

		mm[str]+=num;

		getline(mecabTatalTF, str, '\n');
	}

	mecabTatalTF.close();

	//processing
	while (1)
	{
		cout << "continue?";
		char tmp;
		cin >> tmp;

		if (tmp == 'n') break;

		ifstream mecabTotal (mecabTotalTFName.c_str());

		while (1)
		{
			string str;
			getline(mecabTotal, str, '\t');
			if (mecabTotal.eof()) break;

			int num;
			mecabTotal >> num;
			
			mm[str]+=num;

			getline(mecabTotal, str, '\n');
		}

		mecabTotal.close();
	}

	ofstream result(mecabTotalTFResult.c_str());

	map<string, int>::iterator it;
	for (it = mm.begin(); it != mm.end(); ++it)
		result << it->first << '\t' << it->second << endl;

}