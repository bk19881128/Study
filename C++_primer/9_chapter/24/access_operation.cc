#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>

using namespace std;

int main()
{
	vector<string> vector_string1 = {"123", "456", "789"};

	cout << "fetch the first element:" << endl;
	cout << vector_string1.at(0) << endl;
	cout << vector_string1[0] << endl;
	cout << vector_string1.front() << endl;
	cout << *vector_string1.begin() << endl;

	//test on empty vector
	//vector<string> vector_string2;
	//cout << vector_string2.at(0) << endl;

	return 0;
}	
