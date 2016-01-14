#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
//library-defined function object
#include <functional>
#include "Sales_data.h"

using namespace std;

//run as ./sales_data_to_string_and_double  < ../data/add_item
int main() {

	Sales_data data1, data2;

	cin >> data1 >> data2;
	string s = data1;
	s += string("test");
	cout << s << endl;
	cout << data1 + 1000.0 << endl;

	return 0;
}
