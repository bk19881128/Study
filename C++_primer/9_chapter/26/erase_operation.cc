#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>

using namespace std;

int main()
{
	int ia[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 55, 89};
	
	vector<int> v1;
	v1.assign(ia, ia+sizeof(ia)/sizeof(ia[0]));	

	list<int> l1;
	l1.assign(ia, ia+sizeof(ia)/sizeof(ia[0]));

	for(auto iter = v1.begin(); iter != v1.end();) {
		if(*iter % 2 == 0)
			iter = v1.erase(iter);
		else
			iter++;
	}

	cout << "vector print even:" << endl;
        for(auto iter = v1.begin(); iter != v1.end(); iter++) {
		cout << *iter << " " << endl;
        }

        for(auto iter = l1.begin(); iter != l1.end(); ) {
                if(*iter % 2 == 1)
                        iter = l1.erase(iter);
		else
			iter++;
        }

        cout << "list print odd:" << endl;
        for(auto iter = l1.begin(); iter != l1.end(); iter++) {
                cout << *iter << " " << endl;
        }

	return 0;
}	
