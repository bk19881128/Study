#include <iostream>
#include <stdexcept> //runtime_error
using namespace std;

istream& my_print(istream& input) {
	char in;
	while((input >> in) && (!input.eof())) {
                if(input.bad()) {
			throw runtime_error("IO stream corrupted");
		}
		if(input.fail()) {
			cerr << "bad data, try again" << endl;
			input.clear(istream::goodbit);
			input.ignore(10, ' ');
			continue;
		}
		cout << in;
	}
	input.clear();	
	return input;
}

int main()
{
	my_print(cin);

	return 0;
}	
