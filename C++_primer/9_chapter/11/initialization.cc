#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>

using namespace std;

int main()
{
	//9.11 six ways to create and initialize vector, 2 ways are in 9.13
	vector<int> vector_type1;
	auto iter_type11 = vector_type1.cbegin();
	auto iter_type12 = vector_type1.cend();
	cout << "1: ";
	while(iter_type11 != iter_type12) {
                cout << *iter_type11 << " ";
                iter_type11++;
        }
        cout << endl;

        vector<int> vector_type2{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        auto iter_type21 = vector_type2.cbegin();
        auto iter_type22 = vector_type2.cend();
        cout << "2: ";
        while(iter_type21 != iter_type22) {
                cout << *iter_type21 << " ";
                iter_type21++;
        }
        cout << endl;

        vector<int> vector_type3(vector_type2);
        auto iter_type31 = vector_type3.cbegin();
        auto iter_type32 = vector_type3.cend();
        cout << "3: ";
        while(iter_type31 != iter_type32) {
                cout << *iter_type31 << " ";
                iter_type31++;
        }
        cout << endl;

        vector<int> vector_type4 = vector_type2;
        auto iter_type41 = vector_type4.cbegin();
        auto iter_type42 = vector_type4.cend();
        cout << "4: ";
        while(iter_type41 != iter_type42) {
                cout << *iter_type41 << " ";
                iter_type41++;
        }
        cout << endl;

	//9.13 initialize vector<double> from list<int> or vector<int>
	list<int> list_int = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	vector<int> vector_int = {10, 11, 12, 13, 14};

	cout << "initialize vector<double> from list<int>" << endl;
	vector<double> vector_double(list_int.cbegin(), list_int.cend());
	auto iter1 = vector_double.cbegin();
	auto iter2 = vector_double.cend();
	while(iter1 != iter2) {
		cout << *iter1 << " ";
		iter1++;
	}
	cout << endl;
	
	cout << "initialize vector<double> from vector<int>" << endl;
	vector<double> vector_double2(vector_int.cbegin(), vector_int.cend());
	auto iter3 = vector_double2.cbegin();
	auto iter4 = vector_double2.cend();
	while(iter3 != iter4) {


		cout << *iter3 << " ";
		iter3++;
	}
	cout << endl;
	return 0;
}	
