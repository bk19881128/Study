#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>
#include <iterator>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{

	if(argc != 4) {
		cout << "usage: " <<  argv[0] << " inputfilename outputfile1 outputfile2" << endl;
		return 0;
	}

	ifstream in_file(argv[1]);

	if(!in_file) {
		cout << "input file not exit" << endl;
		return 0;
	}
	ofstream out_file1(argv[2]);
	ofstream out_file2(argv[3]);

	istream_iterator<int> in_iter(in_file);
	istream_iterator<int> eof;

	ostream_iterator<int> out_iter1(out_file1, " ");
	ostream_iterator<int> out_iter2(out_file2, " ");

	while(in_iter != eof) {
		if(*in_iter % 2)
			out_iter1 = *in_iter;
		else
			out_iter2 = *in_iter;
		in_iter++;
	}

	return 0;
}	
