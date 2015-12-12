#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>
#include <map>
#include <utility>
#include <cstddef>
#include <sstream>
#include "myStrBlob.h"
using namespace std;

int main()
{
	StrBlob b1;
	{
		string temp[] = {"a", "an", "the"};
	        StrBlob b2(temp, temp + sizeof(temp)/sizeof(*temp));
		b1 = b2;
		cout << b1.size() << endl;
		b2.push_back("about");
		cout << b2.size() << endl;
	}
	cout << b1.size() << endl;

	cout << "return the front: " << b1.front() << endl;
	cout << "return the back: " << b1.back() << endl;

	

	return 0;
}	
