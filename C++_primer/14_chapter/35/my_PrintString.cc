#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

struct my_PrintString {
	string operator()(istream &is = cin) const { 
		string tmp;
		is >> tmp;
		if(is)
			return tmp;
		else {
			string tmp;
			return tmp;
		}
	}
};

int main() {
	my_PrintString my_PrintStringObj;
	cout << my_PrintStringObj() << endl;
	
	return 0;
}
