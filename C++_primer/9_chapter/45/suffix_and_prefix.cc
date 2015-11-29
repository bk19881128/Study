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
	string s = "Bao";
	const string suffix = "Mr.";
	const string prefix = "SAMA";
	s.insert(s.begin(), suffix.cbegin(), suffix.cend());
	s.append(prefix);
	cout << s << endl;

//another
        s = "Bao";
        s.insert(s.begin(), suffix.cbegin(), suffix.cend());
        s.insert(s.end(), prefix.cbegin(), prefix.cend());
        cout << s << endl;

	return 0;
}	
