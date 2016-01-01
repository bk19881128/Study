#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
//library-defined function object
#include <functional>

using namespace std;

int main() {

	int number = 0;
	greater<int> greater_obj;
	vector<int> v1 = {123123, 2, 43, 1234234};
	for(auto iter = v1.begin(); iter != v1.end(); iter++) {
		if(greater_obj(*iter, 1024))
			number++;
	}
	cout << "the number larger then 1024 is " << number << endl;	

	not_equal_to<string> not_equal_to_obj;
	string s("pooh");
	vector<string> v2 = {"pooh", "pooh", "good", "bad"};
	for(auto iter = v2.begin(); iter != v2.end(); iter++) {
		if(not_equal_to_obj(*iter, s)) {
			cout << "the first string no pooh is " << *iter  << endl;
			break;
		}
	}

	multiplies<int> multiplies_obj;
	vector<int> v3 = {1,2,3,4,5,6};
        for(auto iter = v3.begin(); iter != v3.end(); iter++) {
                *iter = multiplies_obj(*iter, 2); 
        }
        for(auto iter = v3.begin(); iter != v3.end(); iter++) {
                cout << *iter << " ";
        }
	cout << endl;

	return 0;
}
