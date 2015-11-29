#include "../../7_chapter/Sales_data.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
        if(argc != 4 ) {
                cerr << "must use as ./add_item_fileMode [inputfilename] [outputfilename] [outputmode 1:normal, 1:append]" << endl;
                return 0;
        }

	bool filemode = 0;

	if(string(*(argv+3)).compare("0") == 0)
		filemode = 1;
	else if(string(*(argv+3)).compare("1") == 0)
		filemode = 0;
	else{
		cerr << "outputmode 0:normal, 1:append" << endl;
                return 0;
	}

	ifstream input(*(argv+1));

        if(input) {
		Sales_data data1, data2;
		read_file(input, data1);
		read_file(input, data2);

		ofstream output(*(argv+2), filemode ? ofstream::out : ofstream::app);
	        // code to add into data1 and data2 unchanged from chapter 7
	        if (data1.isbn() == data2.isbn()) {     // check isbns
	                Sales_data sum = add(data1, data2); // add the transactions
			print_file(output, sum);
			output << endl;
	        }
        }
        else{
                cerr << "can't open: " + string(*(argv+1)) << endl;
                return 0;
        }

	return 0;
}
