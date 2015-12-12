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

void read_from_console_p(istream &is, vector<int> *p) {
	int s;
	while(is >> s)
		(*p).push_back(s);
}

void print_it_p(ostream &os, vector<int> *p) {
	unsigned int i;
	for(i = 0; i < p->size(); i++)
		os << (*p)[i] << " ";
	os << endl;
}

void read_from_console_s(istream &is, shared_ptr<vector<int> > p) {
        int s;
        while(is >> s)
                (*p).push_back(s);
}

void print_it_s(ostream &os, shared_ptr<vector<int> > p) {
        unsigned int i;
        for(i = 0; i < p->size(); i++)
                os << (*p)[i] << " ";
        os << endl;
}

int main()
{
/*
	auto *p = new vector<int>();
	read_from_console_p(cin, p);
	print_it_p(cout, p);
	delete p;
*/

	shared_ptr<vector<int>> s(new vector<int>);
        read_from_console_s(cin, s);
        print_it_s(cout, s);

	return 0;
}	
