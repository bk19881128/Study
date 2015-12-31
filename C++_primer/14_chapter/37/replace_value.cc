#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

struct ReplaceValue {
	bool operator()(int v1, int v2) const {
		if(v1 == v2)
			return true;
		else
			return false;
	}
};

int main() {

	ReplaceValue ReplaceValueObj;
	//do not know the meaning of the exercise, wait for ...
	cout << ReplaceValueObj(1,2) << endl;
	cout << ReplaceValueObj(2,2) << endl;
	
	return 0;
}
