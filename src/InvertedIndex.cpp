//#define DEBUG
#include "InvertedIndex.h"
#include "fstream"

using namespace std;

/*
* сепарирует полный текст файла на отдельные уникальыне слова и выгружает их в вектор string
* @text - полный текст загруженного файла
*/
static vector<string> wordSepar(const string& text) {
	vector<string> bufVec;
	string word;

	for (auto symbol : text) {

		if (symbol != ' ' && symbol != NULL) {
			word.push_back(symbol);
		}
		else {
			
			bufVec.push_back(word);
			word.clear();
		}
	}
	bufVec.push_back(word);
	word.clear();

	return bufVec;
};

InvertedIndex::InvertedIndex() = default;

InvertedIndex::InvertedIndex(InvertedIndex& idx) : docs(idx.docs), freq_dictionary(idx.freq_dictionary) {};

//вспомогательная функция для разбиения обновления по разным потокам
void InvertedIndex::textProcessingInThread (string& way, int docID) {
	mutex mtx;
#ifdef DEBUG
	mtx.lock();
	cout << "way: " << way << " id: " << docID << endl;
	mtx.unlock();

#endif // DEBUG
	ifstream file(way);
	
	string fileOutputs;
	file >> fileOutputs;
	getline(file, fileOutputs);

	vector<string> words = wordSepar(fileOutputs);
	
	for (auto &wordFromList : words) {
		auto found = freq_dictionary.find(wordFromList);

		if (found == freq_dictionary.end()) { //значит, слова в словаре нет
			Entry e;
			e.doc_id = docID;
			e.count = 1;
			vector<Entry> vec;
			vec.push_back(e);
			freq_dictionary.insert(pair <string, vector<Entry>>(wordFromList, vec));
		}
		else 
		{
			for (auto &entry : freq_dictionary[wordFromList]) {
				if (entry.doc_id == docID) {
					entry.count++;
					break;
				}
				if (entry == freq_dictionary[wordFromList][freq_dictionary[wordFromList].size() - 1]) {
					Entry e;
					e.doc_id = docID;
					e.count = 1;
					freq_dictionary[wordFromList].push_back(e);
				}
			}
		}
	}	
}

// обновление всей базы документов
void InvertedIndex::UpdateDocumentBase(vector<string> input_docs) {
	mutex m;
#ifdef DEBUG
	cout << "UpdateDocumentBase called" << endl;
#endif // DEBUG
	
	//if (!freq_dictionary.empty()) //если список не пуст, то база сносится и создается новая
	{
		//freq_dictionary.clear(); 
		/*
		* в других случаях (когда база крупнее и объемнее) стоило бы хранить где-то в классе имена всех документов с их айдишниками
		* и в момент обновления сравнивать содержимое документов с тем, что хранится где-то в памяти
		*/
	}

	// если список пуст, то происходит его заполнение по новой
	vector <thread> threads; // хранилище всех потоков
	int docID = 1;
	for (auto& way_to_document : input_docs) 
	{
		threads.push_back(thread([&]() {
			textProcessingInThread(way_to_document, docID);
		}));

		this_thread::sleep_for(chrono::milliseconds(1)); // иначе счётчик увеличивается быстрее запуска потоков
		m.lock();
		docID++;
		m.unlock();
	}

	m.lock();
	for (auto& i : threads) {
		i.join();
	}
	m.unlock();
	
#ifdef DEBUG2
	cout << "Freq_dictionaty: " << endl;
	for (auto i : freq_dictionary) {
		cout << "Word: " << i.first << ": " << endl;
		for (auto ii : i.second) {
			cout << "Id: " << ii.doc_id << " count: " << ii.count << endl;
		}
	}
#endif // DEBUG

};

vector<Entry> InvertedIndex::GetWordCount(const string word) 
{
	auto iter = freq_dictionary.find(word);
	
	if (iter != freq_dictionary.end()) {
		return freq_dictionary[word];
	}
	else {		
		vector<Entry> vec; // если слово не найдено - возвращаем пустой вектор и не работаем с ним
		return vec;
	}
};