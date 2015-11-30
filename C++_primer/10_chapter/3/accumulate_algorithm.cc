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

int main()
{

	vector<int> v1 = {0,1,2,1,3,2};
	int result = accumulate(v1.begin(), v1.end(), 1);
	cout << "result is " << result << endl;

	vector<double> v2 = {1.1, 2.2, 3.3, 4.4, 5.5};
	double result1 = accumulate(v2.begin(), v2.end(), 0);
	cout << "result1 is " << result1 << endl;

	return 0;
}	
