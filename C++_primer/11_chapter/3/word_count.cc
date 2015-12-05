#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>
#include <map>
#include <utility>
#include <cstddef>

using namespace std;

int main()
{
	map<string, size_t> word_count;
	string word;

	while(cin >> word) {
		transform(word.begin(), word.end(), word.begin(),  ::tolower);
		++word_count[word];
	}

	for(const auto &w : word_count)
		cout << w.first << " occurs " << w.second
		<< ((w.second > 1) ? " times" : " time") << endl;

	return 0;
}	
