#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

struct testString {
	bool operator()(string& s) const {
		if(s.size() >= 1 && s.size() <= 10)
			return true;
		else
			return false;
	}
};

int main() {

	int number;
	vector<string> v;
	string i;
	while (cin >> i)
		v.push_back(i);

	number = count_if(v.begin(), v.end(), testString());

	cout << "number is " << number << endl;	

	return 0;
}
