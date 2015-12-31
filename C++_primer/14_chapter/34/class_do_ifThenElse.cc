#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

struct ifThenElse {
	string operator()(bool p, string s1, string s2) const {
		if(p)
			return s1;
		else
			return s2;
	}
};

int main() {
	ifThenElse ifThenElseObj;
	cout << ifThenElseObj(1, string("true"), string("false")) << endl;
	cout << ifThenElseObj(0, string("true"), string("false")) << endl;
	
	return 0;
}
