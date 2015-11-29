#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>

using namespace std;

int main()
{
	vector<int> v1 = {1, 2, 3, 4};
	vector<int> v2 = {1, 2, 3, 4, 5};
//9.16
//	list<int> l1 = {1, 2, 3, 4, 5};
	
	if(v1 < v2) {
		cout << "v1 is less then v2" << endl;
	}
	else if(v1 == v2) {
		cout << "v1 is equal to v2" << endl;
	}
	else {
		cout << "v1 is larger than v2" << endl;
	}

/*
        if(v1 < l1) {
                cout << "v1 is less then l1" << endl;
        }
        else if(v1 == l1) {
                cout << "v1 is equal to l1" << endl;
        }
        else {
                cout << "v1 is larger than l1" << endl;
        }
*/

	return 0;
}	
