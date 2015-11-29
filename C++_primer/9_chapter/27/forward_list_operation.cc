#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>

using namespace std;

void my_insert(forward_list<string>& input, string s1, string s2) {
	auto prev = input.before_begin();
	auto cur = input.begin();
	while(cur != input.end()) {
		if(*cur == s1) {
			input.insert_after(cur, s2);
			return;
			++cur;
		}
		prev = cur;
		++cur;
	}
	input.insert_after(prev, s2);
	return;
}

int main()
{
	//9.27
	forward_list<int> flst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	cout << "before remove: " << endl;
        for(auto iter = flst.begin(); iter != flst.end(); iter++)
                cout << *iter << " ";
        cout << endl;

	auto prev = flst.before_begin();
	auto cur = flst.begin();
	while(cur != flst.end()) {
		if(*cur % 2 == 0) {
			cur = flst.erase_after(prev);
		}
		else {
			prev = cur;
			++cur;
		}	
	}

	cout << "after remove: " << endl;
	for(auto iter = flst.begin(); iter != flst.end(); iter++)
		cout << *iter << " ";
	cout << endl;

	//9.28
	forward_list<string> ori = {"hello", "C++", "World"};
        cout << "before insert: " << endl;
        for(auto iter = ori.begin(); iter != ori.end(); iter++)
                cout << *iter << " ";
        cout << endl;

	string string1 = "C++";
	string string2 = "bad";
	my_insert(ori, string1, string2);
        cout << "before remove: " << endl;
        for(auto iter = ori.begin(); iter != ori.end(); iter++)
                cout << *iter << " ";
        cout << endl;

	return 0;
}	
