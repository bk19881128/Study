#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>
#include <map>
#include <utility>
#include <cstddef>
#include <sstream>
using namespace std;

int main()
{
	map<int, int> m = {make_pair(0,1), make_pair(1,2), make_pair(2,3)};
	m[0] = 2;

	cout << m.at(0) << endl;

	vector<int> v ={0, 2,3};
	v[0] = 1;
	cout << v[0] << v[1] << v[2] << endl;

	return 0;
}	
