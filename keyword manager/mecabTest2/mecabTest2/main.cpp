#include <iostream>
#include <mecab.h>
#include <string>
#include <Windows.h>
#include <cstdio>
#include <map>
#include <vector>
#include <algorithm>
#include <time.h>

using namespace std;

map<pair<string, string>, int> msi;
multimap<int, pair<string, string> > mis;
vector<string> vs;


#define CHECK(eval) if (! eval) { \
   const char *e = tagger ? tagger->what() : MeCab::getTaggerError(); \
   std::cerr << "Exception:" << e << std::endl; \
   delete tagger; \
   return -1; }


// Sample of MeCab::Tagger class.
int main(int argc, char **argv) {

	time_t timer = time(NULL);
	tm* t;
	t = localtime(&timer);
	
	string thisTime = to_string(t->tm_year - 100) + "-" + to_string(t->tm_mon + 1) + "-" + to_string(t->tm_mday) + "-"
		+ to_string(t->tm_hour) + "-" + to_string(t->tm_min) + "-" + to_string(t->tm_sec);
	
	string mecabOutputName = "../../../../keyword manager DB/2.mecab_output/" + thisTime + "output.txt";
	string mecabAnalyzeName = "../../../../keyword manager DB/3.final/" + thisTime + "analyzeResult.txt";
	string mecabTotalTFName = "../../../../keyword manager DB/4.total_TF_input/00total_TF_input.txt";
	
	MeCab::Tagger *tagger = MeCab::createTagger("-r C:/librarys/mecab-ko/mecabrc -d C:/librarys/mecab-ko/dic/mecab-ko-dic-1.6.1");
	CHECK(tagger);

	freopen("../../../../keyword manager DB/1.input/input.txt", "r", stdin);
	freopen(mecabOutputName.c_str(), "w+", stdout);

	while (1)
	{
		char input[1000];
		cin.getline(input, 1000);
		if (cin.eof()) break;

		if (!strcmp(input, "")) continue;

		// Gets tagged result in string format.
		const char *result = tagger->parse(input);
		CHECK(result);
		//std::cout << "INPUT: " << input << std::endl;
		//std::cout << "RESULT: " << result << std::endl;

		std::cout << result << std::endl;

		/*
		// Gets N best results in string format.
		result = tagger->parseNBest(3, input);
		CHECK(result);
		std::cout << "NBEST: " << std::endl << result;

		// Gets N best results in sequence.
		CHECK(tagger->parseNBestInit(input));
		for (int i = 0; i < 3; ++i) {
			std::cout << i << ":" << std::endl << tagger->next();
		}

		// Gets Node object.
		const MeCab::Node* node = tagger->parseToNode(input);
		CHECK(node);
		for (; node; node = node->next) {
			std::cout << node->id << ' ';
			if (node->stat == MECAB_BOS_NODE)
				std::cout << "BOS";
			else if (node->stat == MECAB_EOS_NODE)
				std::cout << "EOS";
			else
				std::cout.write(node->surface, node->length);

			std::cout << ' ' << node->feature
				<< ' ' << (int)(node->surface - input)
				<< ' ' << (int)(node->surface - input + node->length)
				<< ' ' << node->rcAttr
				<< ' ' << node->lcAttr
				<< ' ' << node->posid
				<< ' ' << (int)node->char_type
				<< ' ' << (int)node->stat
				<< ' ' << (int)node->isbest
				<< ' ' << node->alpha
				<< ' ' << node->beta
				<< ' ' << node->prob
				<< ' ' << node->cost << std::endl;
		}

		// Dictionary info.
		const MeCab::DictionaryInfo *d = tagger->dictionary_info();
		for (; d; d = d->next) {
			std::cout << "filename: " << d->filename << std::endl;
			std::cout << "charset: " << d->charset << std::endl;
			std::cout << "size: " << d->size << std::endl;
			std::cout << "type: " << d->type << std::endl;
			std::cout << "lsize: " << d->lsize << std::endl;
			std::cout << "rsize: " << d->rsize << std::endl;
			std::cout << "version: " << d->version << std::endl;
		}
		
		//if (input[strlen(input) - 1] == EOF) break;
		*/
	}

	delete tagger;

	fclose(stdin);
	fclose(stdout);

	//---------------------------analyze module--------------------------------

	freopen(mecabOutputName.c_str(), "r", stdin);
	freopen(mecabAnalyzeName.c_str(), "w+", stdout);

	string apstr;
	bool continuous = false;
	int cnt = 0;
	while (1)
	{
		char noun[1000];
		int i = 0;
		bool isEof = false;
		while (1)
		{
			char tmp = getchar();
			if (tmp == EOF)
			{
				isEof = true;
				break;
			}
			if (tmp == '\t'||tmp=='\n') break;
			noun[i++] = tmp;
		}
		if (isEof)break;
		noun[i] = '\0';
		string str = noun;

		if (str=="EOS")
		{
			char tmp = getchar();
			if (tmp == '\n') continue;
			else if (tmp == EOF)
				break;
		}
		string ty;

		while (1)
		{
			char tmp = getchar();
			if (tmp == '\t') continue;
			if (tmp == ',') break;
			ty.append(1, tmp);
		}

		while (1)
		{
			char tmp = getchar();
			if (tmp == '\n') break;
		}

		if ((ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP" || ty == "SL") && continuous == true)
		{
			cout << str << '\t' << ty << endl;
			msi[make_pair(str, ty)]++;
			apstr.append(str + " ");
			cnt++;
		}
		else if (ty == "NNG" || ty == "NNP" || ty == "NNB" || ty == "NNBC" || ty == "NR" || ty == "NP" || ty == "SL")
		{
			cout << str << '\t' << ty << endl;
			msi[make_pair(str, ty)]++;
			continuous = true;
			apstr.append(str + " ");
			cnt++;
		}
		else if (continuous == true && cnt != 1)
		{
			cout << endl;
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
					//cout << appendStr << endl;
					msi[make_pair(appendStr, "MyCompound")]++;
				}
			}
			vs.clear();
			cnt = 0;
		}
		else
		{
			cout << endl;
			continuous = false;
			cnt = 0;
			apstr.erase();
		}
	}

	cout << "-----------------------------------------" << endl;

	string ans;
	int maxx = 0;
	map<pair<string, string>, int>::iterator it;
	for (it = msi.begin(); it != msi.end(); ++it)
	{
		cout << (*it).first.first << '\t' << (*it).first.second
			<< '\t' << (*it).second << endl;
		if (maxx < (*it).second)
		{
			maxx = (*it).second;
			ans = (*it).first.first;
		}
		mis.insert(pair<int, pair<string, string> >((*it).second, (*it).first));
	}

	cout << endl << "------------------------------------" << endl;
	cout << "keyword : " << ans ;
	cout << endl << "------------------------------------";
	cout << endl;

	multimap<int, pair<string, string> >::iterator it2;
	for (it2 = mis.begin(); it2 != mis.end(); ++it2)
		cout << (*it2).second.first << '\t' << (*it2).first << endl;


	fclose(stdin);
	fclose(stdout);

	freopen(mecabTotalTFName.c_str(), "w+", stdout);
	for (it2 = mis.begin(); it2 != mis.end(); ++it2)
		cout << (*it2).second.first << '\t' << (*it2).first << endl;


	return 0;
}
