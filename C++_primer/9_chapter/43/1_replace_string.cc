#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>

using namespace std;

void do_replace(basic_string<char>::iterator iter_begin, basic_string<char>::iterator iter_end, string &s, const string n) {
	
	auto iter = s.erase(iter_begin, iter_end);
	s.insert(iter, n.cbegin(), n.cend());

	return;
}

void replace_string(string &s, const string o, const string n) {

	unsigned int start = 0;
	while (start < s.size()) {
		auto pos = s.find(o, start);
		if (pos != string::npos) {
			auto iter = s.begin();
			do_replace(iter + pos, iter + pos + o.size(), s, n);
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
