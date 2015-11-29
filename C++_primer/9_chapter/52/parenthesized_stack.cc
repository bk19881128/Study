#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>

using namespace std;

int main()
{
	string s1("abcd{w9s}ff{sd f}");
	string s2("WOW");
	stack<char> charStack;
	bool seen = false;
	size_t length = 0;

	for(auto iter = s1.begin(); iter != s1.end(); iter++) {
		if (*iter == '{') {
			seen = true;
			length++;
			charStack.push(*iter);
		}
		else if (*iter == '}') {
			if (seen == true) {
				seen = false;
				for(size_t ix = 0; ix != length; ix++)
					charStack.pop();
				for(auto iter = s2.begin(); iter != s2.end(); iter++)
					charStack.push(*iter);
				length = 0;
			}
		}
		else {
			if(seen ==true)
				length++;
			charStack.push(*iter);
		}

	}

	while(!charStack.empty()) {
		char value = charStack.top();
		cout << value << " ";
		charStack.pop();
	}
	cout << endl;

	

	return 0;
}	
