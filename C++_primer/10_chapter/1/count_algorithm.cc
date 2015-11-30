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

	vector<int> v1 = {0,1,2,1,3,2};
	int number = count(v1.begin(), v1.end(), 1);
	cout << "1 occurs " << number << " times" << endl;
	number = count(v1.begin(), v1.end(), 4);
	cout << "4 occurs " << number << " times" << endl;


	vector<string> v2 = {"hello", "wow", "sdfasd", "wow", "hell"};
	number = count(v2.begin(), v2.end(), "wow");
	cout << "wow occurs " << number << " times" << endl;
	number = count(v2.begin(), v2.end(), "big");
	cout << "big occurs " << number << " times" << endl;
	

	return 0;
}	
