#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>

using namespace std;

void replace_string(string &s, const string o, const string n) {

	auto iter_s = s.begin();
	auto iter_o = oldvalue.begin();

	if(s.size() < o.size() || s.size() < n.size())
		return;

	while (iter_s != s.end()) {
		auto s_bak = iter_s;
		auto o_bak = iter_o;
		while(iter_o != oldvalue.end()) {
			if(*iter_o == *iter_s) {
				iter_s++;
				iter_o++;
				continue;
			}
			break;
		}
		if(iter_o == oldvalue.end()) {
			s.erase(s_bak,iter_s--);
		}
		
	}
} 

int main()
{
	string s = "throthrothro";
	string o = "hro";
	string n = "z";
	replace_string(s, o, n);
	cout << s << endl;
	return 0;
}	
