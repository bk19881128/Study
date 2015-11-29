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
	string numbers("0123456789");	
	string s = "ab2c3d7R4E6";


	cout << "numbers:" << endl;
	unsigned int start = 0;
	while (start < s.size()) {
		auto pos = s.find_first_of(numbers, start);
		if (pos != string::npos) {
			cout << *(s.begin() + pos) << " ";
			start = pos + 1;
		} else
			start++;
	}
	cout << endl;

        cout << "alphabeic:" << endl;
	start = 0;
        while (start < s.size()) {
                auto pos = s.find_first_not_of(numbers, start);
                if (pos != string::npos) {
                        cout << *(s.begin() + pos) << " ";
			start = pos + 1;
                } else
			start++;
        }
        cout << endl;

	return 0;
}	
