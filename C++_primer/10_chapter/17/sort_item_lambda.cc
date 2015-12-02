#include <iostream>

#include "../../7_chapter/Sales_data.h"
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
	vector<Sales_data> v1;
	Sales_data data;
	while(read(cin, data)) {  // read the transactions
		v1.push_back(data);
	}
    
        sort(v1.begin(), v1.end(), [](const Sales_data &data1, const Sales_data &data2){return data1.isbn() >= data2.isbn();});

	for(auto iter = v1.begin(); iter != v1.end(); iter++) {
		print(cout, *iter);
		cout << endl;
        }

	return 0;
}
