#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
//library-defined function object
#include <functional>

using namespace std;

int main() {

	bool result = true;
	modulus<int> modulus_obj;
	//change this to test
	vector<int> v1 = {1, 2, 4, 8};
	int test_value = 9;

	for(auto iter = v1.begin(); iter != v1.end(); iter++) {
		if(modulus_obj(test_value, *iter))
			result = false;
	}
	
	if(result)
		cout << "can be divisible by any element" << endl;
	else
		cout << "can not be divisible by any element" << endl;

	return 0;
}
