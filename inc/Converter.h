#include <string>
#include <vector> 

#include <nlohmann/json.hpp>

#include "Structures.h"

using namespace std;

class ConverterJSON {
public:
	ConverterJSON();

	vector<string> GetTextDocuments();

	int GetResponsesLimit();

	vector<string> GetRequests();

	void putAnswers(vector<vector<RelativeIndex>>& answers);

	void showInfo();
};