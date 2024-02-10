#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <memory>

#include "InvertedIndex.h" // в нем уже есть Structures 



#pragma once

class SearchServer {

	InvertedIndex* _index;

	vector<RelativeIndex> serchingInThread(string request);

public:

	SearchServer(InvertedIndex* idx);

	vector<vector<RelativeIndex>> search
	(const vector<string>& queries_input);
};