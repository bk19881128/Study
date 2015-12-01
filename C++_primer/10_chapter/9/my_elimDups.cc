#include <algorithm>
using std::sort; using std::for_each;

#include <string>
using std::string;

#include <vector>
using std::vector; 

#include <iostream>
using std::cin; using std::cout; using std::endl; 

// comparison function to be used to sort by word length
bool isShorter(const string &s1, const string &s2)
{
    return s1.size() < s2.size();
}

void printStr(const string &s) {
	cout << s << " ";
}

void print(const vector<string> &words)
{
	for_each(words.begin(), words.end(), printStr);
	cout << endl;
}

int main()
{
    vector<string> words;

    //10.09
    // copy contents of each book into a single vector
    string next_word;
    while (cin >> next_word) {
        // insert next book's contents at end of words
        words.push_back(next_word);
    }
	print(words);

        vector<string> cpy = words; // save the original data

	// uses string < to compare elements
	// sort and print the vector
	sort(words.begin(), words.end());

	print(words);

	auto end_unique = unique(words.begin(), words.end());

	print(words);

	words.erase(end_unique, words.end());

	print(words);

       //10.11
       stable_sort(cpy.begin(), cpy.end(), isShorter);
	print(cpy);
	return 0;
}
