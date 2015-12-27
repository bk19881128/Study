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

int f() {
	return 5;
}

int main() {
	vector<int> vi(100);
	int &&r1 = f();
	int &r2 = vi[0];
	int &r3 = r1;
	int &&r4 = vi[0] * f();

	return 0;
}
