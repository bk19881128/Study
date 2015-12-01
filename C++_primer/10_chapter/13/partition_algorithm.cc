#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>

using namespace std;

bool islonger(string& s1) {
	if(s1.size() >= 5)
		return true;
	else
		return false;
}

int main()
{

	vector<string> v = {"123456", "abcde", "acb"};
	auto p_iter = partition(v.begin(), v.end(), islonger);
	
	cout << "long string:" << endl;
	for(auto iter = v.begin(); iter != p_iter; iter++)
		cout << *iter << " ";
	cout << endl;

        cout << "short string:" << endl;
        for(auto iter = p_iter; iter != v.end(); iter++)
                cout << *iter << " ";
	cout << endl;

	return 0;
}	
