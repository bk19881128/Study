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

int main()
{

	istream_iterator<int> in_iter(cin);
	istream_iterator<int> eof;
	vector<int> v1;

	ostream_iterator<int> out(cout, " ");
	
	while(in_iter != eof) {
		v1.push_back(*in_iter++);
	}

	sort(v1.begin(), v1.end());
	copy(v1.begin(), v1.end(), out);
	cout << endl;

	return 0;
}	
