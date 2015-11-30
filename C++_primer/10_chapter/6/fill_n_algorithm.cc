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

int main()
{

	vector<int> v1 = {0,1,2,3,4,5,6,7,8,9};

	cout << "before fill_n" << endl;
	for(auto iter = v1.cbegin(); iter != v1.cend(); iter++)
		cout << *iter << " ";
	cout << endl;

	fill_n(v1.begin(), v1.size(), 0);

        cout << "after fill_n" << endl;
        for(auto iter = v1.cbegin(); iter != v1.cend(); iter++)
                cout << *iter << " ";
	cout << endl;
	return 0;
}	
