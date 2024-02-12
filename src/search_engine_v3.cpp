//#define DEBUG
#define TESTING

#include <iostream>
#include <fstream>

#include "Converter.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include "gtest/gtest.h"

#ifdef TESTING
	TEST(sample_test_case, sample_test)
	{
		EXPECT_EQ(1, 1);
	}
#endif // TESTING





using namespace std;

void cycleUpdates (InvertedIndex &index, ConverterJSON &converter) {
	while (true) {
		this_thread::sleep_for(chrono::minutes(5));
		index.UpdateDocumentBase(converter.GetTextDocuments());
	}
}

int main()
{
#ifdef DEBUG
	cout << "DEBUG!" << endl;
#endif // DEBUG


	cout << "Start:" << endl;
	mutex mtx;

	ConverterJSON converter;
	InvertedIndex index;
	SearchServer server (&index);

	converter.showInfo(); // выводим информацию о программе
	cout << endl;
	

	
	string command;

	mtx.lock();
	cout << "Interface:\n" <<
		"\"search\"\t- call metod \"search\" for file requests.json;\n" <<
		"\"info\"\t\t- output information of name and version programm;\n" << 
		"\"update\"\t- forced system update;\n" << 
		"\"commands\"\t- show commands list.\n" << endl;
	mtx.unlock();

	

	
	index.UpdateDocumentBase(converter.GetTextDocuments());
#ifdef DEBUG
	cout << "Base has update!" << endl;
#endif // DEBUG


	thread thrForUpdateBase(cycleUpdates, index, converter);
	thrForUpdateBase.detach();

	do {
		cout << "Enter the command: ";
		cin >> command;

		if (command != "stop" || command != "Stop") {

			if (command == "search") {
				converter.putAnswers(server.search(converter.GetRequests()));
			}
			else if (command == "info") {
				converter.showInfo();
			}
			else if (command == "update") {
				index.UpdateDocumentBase(converter.GetTextDocuments());
			}
			else if (command == "commands") {
				cout << "Interface:\n" <<
					"\"search\"\t- call metod \"search\" for file requests.json;\n" <<
					"\"info\"\t- output information of name and version programm;\n" <<
					"\"update\"\t- forced system update;\n" <<
					"\"commands\"\t- show commands list." << endl;
			}
		}
	
	} while (command != "stop");
	cout << "programm stopped" << endl;
	return 0;
}
