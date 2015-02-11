#include <iostream>
#include <Windows.h>
#include <map>
#include <string>
#include <fstream>
#include <math.h>
#include <limits>
#include <time.h>

using namespace std;

#define SERVERIP "127.0.0.1"
#define PORT 74132

map<pair<string,string>, double> stringTypeNum;

int main()
{
	cout << "***  강석일 회원의 keyword DB를 쌓겠습니다..  ***" << endl;


	WSADATA wsaData;
	SOCKET hSocket;
	char msg[100];
	int strlen;
	SOCKADDR_IN servAdr;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	hSocket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(PORT);
	servAdr.sin_addr.s_addr = inet_addr(SERVERIP);

	//*****
	
	if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		cout << "connect error" << endl;
	else
		cout << "connected!" << endl;
	
	//******

	
	//***
	string mecabTotalTFName = "../../../../keyword manager DB/4.total_TF_input/00.total_TF_input.txt";
	//string mecabTotalTFName = "../../../../keyword manager DB1/3-3-0.final_increase_TF/00.analyzeResult.txt";
	//***

	string mecabTotalTFResult = "../../../../keyword manager DB/5.total_TF/01.total_TF_result.txt";
	string mecabCalculateIDF = "../../../../keyword manager DB/6.caclulate_IDF/00.caculate.txt";
	
	//preprocess
	ifstream ImecabTotalTF(mecabTotalTFResult);

	string ttmp;
	getline(ImecabTotalTF, ttmp, '\n');

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
		cout << "CONTINUE?"<<endl;
		
		/*
		string tmp;
		cin >> tmp;
		if (tmp == "n") break;
		*/
		//******
		int len = recv(hSocket, msg, 99, 0);
		msg[len] = 0;
		//******


		if (!strcmp(msg, "0")){
			cout << "종료중..." << endl;
			break;
		}

		documentN++;

		//****
		string mecabSelectivDF = "../../../../keyword manager DB/5-1.selectiveDF/00.selectve.txt";
		//string mecabSelectivDF = "../../../../keyword manager DB1/5-1-0.selectiveDF_TF/00.selectve.txt";
		//****

		

		ifstream ImecabTotal(mecabTotalTFName);
		ofstream OselectiveDF(mecabSelectivDF);

		OselectiveDF << endl << documentN << endl;

		//DF값 추가.
		while (1)
		{
			string str;
			getline(ImecabTotal, str, '\t');
			if (ImecabTotal.eof()) break;

			string ty;
			ImecabTotal >> ty;

			//***
			int num;
			ImecabTotal >> num;
			//double TF;
			//ImecabTotal >> TF;
			//***

			

			double selectedNum = stringTypeNum[make_pair(str, ty)];

			//***
			OselectiveDF << str << '\t' << ty << '\t' << selectedNum+1 << endl;
			//***
			//OselectiveDF << str << '\t' << ty << '\t' << selectedNum + 1 << '\t' << TF << endl;
			
			stringTypeNum[make_pair(str, ty)] += 1;

			//'\n' 제거
			getline(ImecabTotal, str, '\n');
		}

		ImecabTotal.close();	

		//*****
		char msg2[100] = "2";
		send(hSocket, msg2, 99, 0);
		//******
	}

	//추가된 DF를 기록.
	ofstream Oresult(mecabTotalTFResult);

	Oresult << endl << documentN << endl;
	map<pair<string, string>, double>::iterator it;
	for (it = stringTypeNum.begin(); it != stringTypeNum.end(); ++it)
		Oresult << it->first.first << '\t' << it->first.second << '\t' << it->second << endl;


	Oresult.close();
}