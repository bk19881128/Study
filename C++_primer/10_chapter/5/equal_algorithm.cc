#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>

using namespace std;

int main()
{

        list<const char*> l1 = {"ab", "cd"};
        list<const char*> l2 = {"ab", "cd"};
        if(equal(l1.cbegin(), l1.cend(), l2.cbegin()) == true)
                cout << "l1 and l2 are equal" << endl;
        else
                cout << "l1 and l2 are unequal" << endl;

	return 0;
}	
