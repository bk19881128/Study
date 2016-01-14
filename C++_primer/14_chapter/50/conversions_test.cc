#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
//library-defined function object
#include <functional>

using namespace std;

struct LongDouble {
	LongDouble () { };
	operator double();
	operator float() {return 0.0;}
};

int main() {

	LongDouble ldObj;
//	int ex1  = ldObj;
	float ex2 = ldObj;
	cout << ex2 << endl;

	return 0;
}
