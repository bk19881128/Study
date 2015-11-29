#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>

using namespace std;

#define VALUE_TO_FIND 4

bool find_value_in_vector(vector<int>::const_iterator iter1, vector<int>::const_iterator iter2, int value) {
	while(iter1 != iter2) {
		if(*iter1 == value)
			return true;
		else
			++iter1;
	}
	return false;
}

vector<int>::const_iterator find_value_in_vector_iterator(vector<int>::const_iterator iter1, vector<int>::const_iterator iter2, int value){
        while(iter1 != iter2) {
                if(*iter1 == value)
                        return iter1;
                else
                        ++iter1;
        }
        return iter1;
}

int main()
{
	list<deque<int>> test_9_2; //just a test for exercise 9.2

	vector<int> test_number;
	for(int i = 0; i < 10; i++)
		test_number.push_back(i);

	bool result = find_value_in_vector(test_number.cbegin(), test_number.cend(), VALUE_TO_FIND);

	if (result == true)
		cout << "can find " << VALUE_TO_FIND << " in the vector" << endl;
	else
		cout << "can not find " << VALUE_TO_FIND << " in the vector" << endl; 
	
	vector<int>::const_iterator iter = find_value_in_vector_iterator(test_number.cbegin(), test_number.cend(), VALUE_TO_FIND);

        if (iter == test_number.cend())
                cout << "can not find " << VALUE_TO_FIND << " in the vector(v2)" << endl;
        else
                cout << "can find " << VALUE_TO_FIND << " in the vector(v2)" << endl;

	return 0;
}	
