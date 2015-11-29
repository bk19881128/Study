#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>

using namespace std;

int main()
{

	vector<char> vc = {'a', 'b', 'c'};
	string s(vc.begin(), vc.end());
	cout << s << endl;

	return 0;
}	
