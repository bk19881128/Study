#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>

using namespace std;

int main()
{
	vector<string> v = {"1", "2", "3", "4"};
	int sum = 0;
	for(auto iter = v.begin(); iter != v.end(); iter++) {
		sum += stoi(*iter);
	}
	cout << "sum is " << sum << endl;

	vector<string> v2 = {"1.1", "2.2","3.3", "4.4", "5.5"};
	float sum1 = 0.0;
        for(auto iter = v2.begin(); iter != v2.end(); iter++) {
                sum1 += stof(*iter);
        }
        cout << "sum1 is " << sum1 << endl;

	return 0;
}	
