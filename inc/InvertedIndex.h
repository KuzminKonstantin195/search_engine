#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <vector>
#include <map>
#include <memory>

#include "Structures.h"


#pragma once

using namespace std;



class InvertedIndex {
	vector<string> docs;                         

	//mutex mtx;
public:
	//частотный словарь
	map<string, vector<Entry>> freq_dictionary; 

	InvertedIndex();

	InvertedIndex(InvertedIndex& idx);

	//вспомогательная функция для обработки в отдельном потоке
	void textProcessingInThread(string& way, int docID);

	void UpdateDocumentBase(vector<string> input_docs);

	vector<Entry> GetWordCount(const string word);
};
