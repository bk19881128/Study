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

	vector<int> v1 = {1,2,3,4,5,6,7,8,9};
	
	list<int> l1, l2, l3;

	copy(v1.cbegin(), v1.cend(), front_inserter(l1));

	cout << "front_inserter" << endl;
	for(auto iter = l1.begin(); iter != l1.end(); iter++)
		cout << *iter << " ";
	cout << endl;

	copy(v1.cbegin(), v1.cend(), back_inserter(l2));

        cout << "back_inserter" << endl;
        for(auto iter = l2.begin(); iter != l2.end(); iter++)
                cout << *iter << " ";
        cout << endl;

        copy(v1.cbegin(), v1.cend(), inserter(l3, l3.begin()));

        cout << "inserter" << endl;
        for(auto iter = l3.begin(); iter != l3.end(); iter++)
                cout << *iter << " ";
        cout << endl;




	return 0;
}	
