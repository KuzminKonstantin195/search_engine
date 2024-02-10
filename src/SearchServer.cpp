#pragma once
#include "SearchServer.h"
//#define DEBUG

using namespace std;

//разбирает входящую сплошную строку на отдельные слова
static vector<string> wordSepar(string& text) {
	vector<string> toReturn;
	string word;
	for (auto h : text) {
		if (h != ' ') {
			word.push_back(h);
		}
		else {
			toReturn.push_back(word);
			word.clear();
		}
	}
	toReturn.push_back(word);
	return toReturn;
}

//оставляет в получаемом векторе только уникальные значения
static void sort_unic (vector <string>& words) {
	for (int wordNum = 0; wordNum < words.size(); wordNum++) {
		for (int i = wordNum + 1; i < words.size(); i++) {
			if (words[wordNum] == words[i]) {
				words.erase(words.begin() + i);
				i--;
			}
		}
	}
}

static void merge(vector<string>& words, InvertedIndex& idx, int start, int end, int mid);


//сортирует входящий вектор по частоте вхождений слова в индексированном списке
static void sort_frequency(vector<string>& words, InvertedIndex& idx, int start, int end)
{
	if (start < end) { // проверка введенных данных
		if (end - start == 1) { // значит в векторе два элемента

			if (idx.GetWordCount(words[start]).size() > idx.GetWordCount(words[end]).size()) {
				swap(words[start], words[end]);
			}
		}
		else {
			int mid = (start + end) / 2;
			sort_frequency(words, idx, start, mid);
			sort_frequency(words, idx, mid + 1, end);
			merge(words, idx, start, end, mid);
			
		}
	}
}

static void merge(vector<string>& words, InvertedIndex& idx, int start, int end, int mid)
{
	int i_left = start, i_right = mid + 1;

	auto buffer = new vector<string>;

	while (i_left <= mid && i_right <= end) {
		if (idx.GetWordCount(words[i_left]).size() <= idx.GetWordCount(words[i_right]).size())
		{
			buffer->push_back(words[i_left]);
			i_left++;
		}
		else
		{
			buffer->push_back(words[i_right]);
			i_right++;
		}
	}

	while (i_left <= mid)
	{
		buffer->push_back(words[i_left]);
		i_left++;
	}
	while (i_right <= end)
	{
		buffer->push_back(words[i_right]);
		i_right++;
	}

	for (int i = 0; i < buffer->size(); i++)
	{
		words[start + i] = (*buffer)[i];
	}
}


SearchServer::SearchServer(InvertedIndex* idx) : _index(idx) {};


/*
* Метод обработки поисковых запросов
* @param queries_input поисковые запросы взятые из файла requests.json
* @return возвращает отсортированный список релевантных ответов для заданных запросов
*/
vector<vector<RelativeIndex>> SearchServer::search
(const vector<string>& queries_input) 
{
	vector<vector<RelativeIndex>> toReturn;
	vector<thread> threadsVec;

	for (auto& request : queries_input) {
		threadsVec.push_back (thread ([&]() {toReturn.push_back(serchingInThread(request)); }));
	}

	for (auto& i : threadsVec) {
#ifdef DEBUG
		cout << "Thread " << i.get_id();
#endif // DEBUG		
		
		i.join();

#ifdef DEBUG
		cout << i.get_id() << " joined" << endl;
#endif // DEBUG
	}

	return toReturn;
}

vector<RelativeIndex> SearchServer::serchingInThread(string request)
{
	mutex mtx;

#ifdef DEBUG
		mtx.lock();
		cout << "Thread num: " << this_thread::get_id() << " has started" << endl;
		mtx.unlock();
#endif // DEBUG
	vector<string> wordsInRequest = wordSepar(request); // разделяет текст на слова

	sort_unic(wordsInRequest); // отсекает повторы слов

	sort_frequency(wordsInRequest, *_index, 0, wordsInRequest.size() - 1); // сортировка по возрастанию встречаемости


	vector<int> IDs; 
	//счётчик слов из запроса, которые не были найдены
	int wordsNotFoundCount = 0;

	for (auto& word : wordsInRequest) {
#ifdef DEBUG
		mtx.lock();
		cout << "Word is: \"" << word << "\"" << endl;
		mtx.unlock();
#endif // DEBUG

		auto entrList = _index->GetWordCount(word);
		
#ifdef DEBUG
		mtx.lock();
		cout << "EntrList size for " << word << " : " << entrList.size() << endl;
		mtx.unlock();
#endif // DEBUG
		
		
		if (!entrList.empty()) // проверка на наличие данных в векторе
		{
			if (IDs.empty()) {
#ifdef DEBUG
				mtx.lock();
				cout << "id in entrList[" << word << "]:";
				mtx.unlock();
#endif // DEBUG
				for (auto& i : entrList) {
					mtx.lock();
						IDs.push_back(i.doc_id);
					mtx.unlock();
#ifdef DEBUG
					mtx.lock();
					cout << "id: " << i.doc_id << " has added to IDs" << endl;
					mtx.unlock();
#endif // DEBUG
				}
				continue;
			}

			vector<int> bufIDs;
			for (auto id_fromIDs : IDs)
			{
				for (auto& i : entrList)
				{
					if (id_fromIDs == i.doc_id) {

#ifdef DEBUG
						mtx.lock();
						cout << "id: " << i.doc_id << " has added to bufferIDs" << endl;
						mtx.unlock();
#endif // DEBUG

						bufIDs.push_back(id_fromIDs);
					}
				}
			}

			if (!bufIDs.empty()) {
				mtx.lock();
				IDs = bufIDs;
				mtx.unlock();
			}
			else
			{
				
				for (auto& i : entrList)
				{
					bufIDs.push_back(i.doc_id);
				}
				
				
			}
		}
		else {
			//блок на случай, если по какому-то слову не найдено совпадение в базе
			wordsNotFoundCount++;
		}
		//в итоге в IDs содержатся только номера документов, в которых содержатся ВСЕ слова из запроса
	}

#ifdef DEBUG
	mtx.lock();
	cout << "IDs in thread" << this_thread::get_id() << ": ";
	for (auto i : IDs) {
		cout << i << " ";
	}
	cout << endl;
	mtx.unlock();
#endif // DEBUG

	//***********************************************************************************************//
	vector<RelativeIndex> vecIndexForOneQuestion;
	int maxRevelation = 0;
	for (auto id : IDs) {
		RelativeIndex indexID;
		indexID.doc_id = id;
		indexID.rank = 0;

		for (auto word : wordsInRequest) {
			for (auto& i : _index->GetWordCount(word)) {
				if (i.doc_id == id) {
					indexID.rank += i.count;
				}
			}
		}

		if (indexID.rank > maxRevelation) {
			maxRevelation = indexID.rank;
		}
		vecIndexForOneQuestion.push_back(indexID);
	}
	//преобразуем интовые совпадения в относительную релевантность
	for (auto& i : vecIndexForOneQuestion) {
		i.rank = i.rank / (maxRevelation+wordsNotFoundCount);
	}
#ifdef DEBUG
	cout << "serchingInThread in thread " << this_thread::get_id() << " ending!" << endl;
#endif // DEBUG

	return vecIndexForOneQuestion;
}
