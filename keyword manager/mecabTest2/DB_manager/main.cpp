#include <iostream>
#include <Windows.h>
#include <map>
#include <string>

using namespace std;

map<string, int> mm;

int main()
{
	string mecabTotalTFName = "../../../../keyword manager DB/4.total_TF_input/00total_TF_input.txt";
	string mecabTotalTFResult = "../../../../keyword manager DB/5.total_TF/00.total_TF_result.txt";

	while (1)
	{
		freopen(mecabTotalTFName.c_str(), "r", stdin);

		while (1)
		{
			string str;
			getline(cin, str, '\t');
			if (cin.eof()) break;

			int num;
			cin >> num;

			mm[str]++;

			getchar();

		}

		int kk = fclose(stdin);
		
		cout << "continue?";
		char tmp;
		cin >> tmp;
		int kkd;
		cin >> kkd;
		if (tmp == 'n') break;
	}

	freopen(mecabTotalTFResult.c_str(), "w+", stdout);
	map<string, int>::iterator it;
	for (it = mm.begin(); it != mm.end(); ++it)
		cout << it->first << " " << it->second << endl;

}