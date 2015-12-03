#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>
#include <iterator>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{

	if(argc != 2) {
		cout << "usage: " <<  argv[0] << " filename" << endl;
		return 0;
	}

	ifstream in_file(argv[1]);

	istream_iterator<string> in_iter(in_file);
	istream_iterator<string> eof;

	vector<string> v1(in_iter, eof);

	for(auto iter = v1.cbegin(); iter != v1.cend(); iter++)
		cout << *iter << " ";
	cout << endl;

	return 0;
}	
