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
#include <sstream>
using namespace std;

int main()
{
	string s1, s2;
	cout << "please input string 1: " << endl;
	cin >> s1;

	cout << "please input string 2: " << endl;
	cin >> s2;

	s1 = s1 + s2;

	char *p = new char[s1.size()];
	int i = 0;
	for(auto iter = s1.begin(); iter != s1.end(); iter++) {
		p[i] = *iter;
		i++;
	}

	for (char *q = p; q != p + s1.size(); ++q)
		cout << *q;
	cout << endl;

	delete [] p;
	return 0;
}	
