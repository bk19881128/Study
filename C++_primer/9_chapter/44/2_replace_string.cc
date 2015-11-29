#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>

using namespace std;

void replace_string(string &s, const string o, const string n) {

	unsigned int start = 0;
	while (start < s.size()) {
		auto pos = s.find(o, start);
		if (pos != string::npos) {
			s.replace(pos, o.size(), n);
		}
		start++;
	}
	return;
} 

int main()
{
	//after replace:tztztz
	string s = "throthrothro";
	const string o = "hro";
	const string n = "z";
	replace_string(s, o, n);
	cout << s << endl;
	return 0;
}	
