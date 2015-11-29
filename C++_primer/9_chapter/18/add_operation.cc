#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 2) {
		cout << "Usage: ./add_operation 1/2/3" << endl;
		cout << "1: 9.18" << endl;
		cout << "2: 9.19" << endl;
		cout << "3: 9.20" << endl;
	}

	string input;
	deque<string> d1;
	list<string> l1;

	switch(atoi(*(argv+1))) {
		case 1:
			//9.18
			{
			cout << "please input string <deque>:" << endl;
			while(cin >> input) {
				d1.push_back(input);
			}
			cout << "print: " << endl;	
			auto iter1 = d1.cbegin();
			auto iter2 = d1.cend();
			while(iter1 != iter2) {
			cout << *iter1 << endl;
			++iter1;
			}
			cout << endl;
			break;
			}

		case 2:
			//9.19
			{
		        cout << "please input string <list>:" << endl;
		        while(cin >> input) {
		                l1.push_back(input);
		        }
			cout << "print: " << endl;
		        auto iter3 = l1.cbegin();
		        auto iter4 = l1.cend();
		        while(iter3 != iter4) {
		                cout << *iter3 << endl;
	        	        ++iter3;
		        }
			break;
			cout << endl;
			}
		case 3:
			//9.20
			{
			list<int> l2 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
			auto iter5 = l2.cbegin();
			auto iter6 = l2.cend();
			deque<int> deque_even, deque_odd;
			while(iter5 != iter6) {
				if(*iter5 % 2 == 0)
					deque_odd.push_back(*iter5);
				else
					deque_even.push_back(*iter5);
				iter5++;
			}
			cout << "print odd: ";		
			auto iter7 = deque_odd.cbegin();
			auto iter8 = deque_odd.cend();
			while(iter7 != iter8) {
				cout << *iter7 << " ";
				iter7++;
			}
			cout <<  endl;

                        cout << "print even: ";          
                        auto iter9 = deque_even.cbegin();
                        auto iter10 = deque_even.cend();
                        while(iter9 != iter10) {
                                cout << *iter9 << " ";
				iter9++;
                        }
                        cout <<  endl;
			break;
                        }

		default:
			cout << "mode must be 1/2/3" << endl;
	}

	return 0;
}	
