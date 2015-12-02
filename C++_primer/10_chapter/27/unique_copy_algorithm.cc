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

	vector<int> v1 = {0,0,0,1,3,2,2,4,5,6,7,9,8,8};
	list<int> l1(v1.size());

	unique_copy(v1.cbegin(), v1.cend(), l1.begin());

	for(auto iter = l1.begin(); iter != l1.end(); iter++)
		cout << *iter << " ";
	cout << endl;

	return 0;
}	
