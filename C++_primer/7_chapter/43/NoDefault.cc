#include <string>
#include <iostream>
#include <vector>

using namespace std;

class NoDefault {
public:
//	NoDefault() : datafield(0) { }
	NoDefault(int s) : datafield(s) { }
private:
	int datafield;
};

struct C {
	C() : my_mem(0)  { }
	NoDefault my_mem;
};

int main() {
	C test;

//	vector<NoDefault> vect(10);
	vector<C> vect(10);
}
