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
	//list
	cout << "list operation" << endl;
	list<int> l1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        cout << "before operation: " << endl;
        for(auto iter = l1.begin(); iter != l1.end(); iter++)
                cout << *iter << " ";
        cout << endl;
	
	auto iter = l1.begin();
	while( iter != l1.end() ){
		if(*iter % 2) {
			//return insert pos, before cur
			iter = l1.insert(iter, *iter++);
	//		iter++;
			iter++;
		}
		else {
			iter = l1.erase(iter);
		}
	}
        cout << "after operation: " << endl;
        for(auto iter = l1.begin(); iter != l1.end(); iter++)
                cout << *iter << " ";
        cout << endl;

	//forward list
	cout << "forward list operation" << endl;
	forward_list<int> flst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        cout << "before operation: " << endl;
        for(auto iter = flst.begin(); iter != flst.end(); iter++)
                cout << *iter << " ";
        cout << endl;

	auto prev = flst.before_begin();
	auto cur = flst.begin();
	while (cur != flst.end()) {
		if(*cur % 2 == 0) {
			cur = flst.erase_after(prev);
		}
		else {
			//return the insert pos, after cur
			cur = flst.insert_after(cur, *cur);
			prev = cur;
                        cur++;
		}
	}

        cout << "after operation: " << endl;
        for(auto iter = flst.begin(); iter != flst.end(); iter++)
                cout << *iter << " ";
        cout << endl;

	return 0;
}	
