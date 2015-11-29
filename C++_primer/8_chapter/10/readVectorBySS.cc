#include <iostream>
#include <stdexcept> //runtime_error
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 2) {
		cerr << "must use as ./readFile_lineToVector [filename]" << endl;
		return 0;
	}

	ifstream input(*(argv+1));
	vector<string> content;
	string buf; 
	if(input) {
		while(getline(input, buf))
			content.push_back(buf);
	}
	else{
		cerr << "can't open: " + string(*(argv+1)) << endl;
		return 0;
	}

	cout << "print vector: " << endl;

	istringstream input_record;

	for(unsigned int i = 0; i < content.size(); i++) {
		input_record.str(content[i]);
		cout << input_record.str() << endl;
        }
	
	return 0;
}	
