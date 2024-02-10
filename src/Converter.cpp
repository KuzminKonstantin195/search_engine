//#define DEBUG

#include "Converter.h"

#include <fstream>
#include <iostream>
#include <cmath> // для round

#pragma once

using namespace std;
using namespace nlohmann;

ConverterJSON::ConverterJSON() = default;

//выгружает пути к файлам базы данных в принимаемый vector<string>
vector<string> ConverterJSON::GetTextDocuments() {
#ifdef DEBUG
	cout << "ConverterJSON::GetTextDocuments() has called" << endl;
#endif // DEBUG

	vector<string> vec;
	ifstream file_config;
	try
	{
		file_config.open("./config.json");
		

	}
	catch (const std::exception& ex)
	{
		cout << "Config don't open!" << endl;
		cout << "Exception: " << ex.what() << endl;
		return vec;
	}


	json js;
	file_config >> js;
	js = js["files"];

	for (auto i : js) {
#ifdef DEBUG
		cout << "way: " << i << endl;
#endif // DEBUG

		vec.push_back((string)i);
	}
	file_config.close();
	
	return vec;
};

//получаем лимит показываемых документов для одного запроса
int ConverterJSON::GetResponsesLimit() {
#ifdef DEBUG
	cout << "ConverterJSON::GetResponsesLimit() has called" << endl;
#endif // DEBUG
	ifstream file_config("./config.json");

	json js;
	file_config >> js;
	js = js["config"];
	file_config.close();

	return js["max_responses"];
};

//получаем вопросы 
vector<string> ConverterJSON::GetRequests() {
#ifdef DEBUG
	cout << "ConverterJSON::GetRequests() has called" << endl;
#endif // DEBUG
	vector<string> vec;
	ifstream file;

	try
	{
		file.open("requests.json");
	}
	catch (const std::exception&)
	{
		cout << "requests don't open!" << endl;
		return vec;
	}
	json requests;
	file >> requests;
	requests = requests["requests"];
	for (auto i : requests) {
		vec.push_back((string) i);
	}
	file.close();
	return vec;
};

// выгрузка ответов в формате (вопрос№ - индекс совпадения)
void ConverterJSON::putAnswers(vector<vector<RelativeIndex>>& answers) {

	ofstream file;
	try
	{
		file.open("./answers.json");
#ifdef DEBUG
		cout << "answers.json has opened!" << endl;
#endif // DEBUG

	}
	catch (const std::exception&)
	{
		cout << "answers don't open!" << endl;
	}
	
	int resCount = GetResponsesLimit();
#ifdef DEBUG
	cout << "Responce limit: " << resCount << endl;
	cout << "Answers size: " << answers.size() << ", answers.answers size: " << answers[0].size() << endl;
#endif // DEBUG

	json jsAnswers;

	for (int i = 0; i < answers.size(); i++) {
		json jsOnesReq;

		string num;

		if (i > 99) 
		{ 
			num = to_string(i+1);
		}
		else if (i < 100 && i > 9) 
		{
			num = "0" + to_string(i+1);
		}
		else 
		{
			num = "00" + to_string(i+1);
		}

		num = "request" + num;
#ifdef DEBUG
		cout << "Num: " << num << endl;
#endif // DEBUG

		if (answers[i].size() == 0) {
			jsOnesReq["result"] = false;
#ifdef DEBUG
			cout << "jsOnesReq: " << jsOnesReq << endl;
#endif // DEBUG
		}
		else {
			jsOnesReq["result"] = true;

			int iterations = (resCount < answers[i].size() ? resCount : (int)answers[i].size());

			for (auto doc = 0; doc < iterations; doc++) {
				json buf;
				buf["docid"] = answers[i][doc].doc_id;
				int rounded = 3;
				string rank = to_string(round(answers[i][doc].rank * (10 * rounded)) / (10 * rounded));
				rank.resize(rounded+2);
				buf["rank"] = rank;
				jsOnesReq["relevations"] += buf;
			}
#ifdef DEBUG
			cout << "jsOnesReq: " << jsOnesReq << endl;;
#endif // DEBUG
			jsAnswers[num] = jsOnesReq;
		}
	}
	file << jsAnswers;

	file.close();
//#ifdef DEBUG
	cout << "The file \"answers.json\" has been writting" << endl;
//#endif // DEBUG

};

//получить информацию о названии и версии программы
void ConverterJSON::showInfo() {

#ifdef DEBUG
	cout << "ConverterJSON::showInfo() has called" << endl;
#endif // DEBUG
	
	json js;
	ifstream file("./config.json");

	file >> js;
	js = js["config"];
	cout << "Name: " << js["name"] << ";\nVersion: " << js["version"] << "." << endl;
	
};