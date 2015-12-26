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

static size_t  sn;

class Employee {
public:
	Employee () : employee_ID(++sn) { }
	Employee (string a) : employee_name(a), employee_ID(++sn) { }
	Employee (const Employee&) = delete;
	Employee &operator = (const Employee&) = delete;
	~Employee () = default;

	string getEmployeeName() {return employee_name;}
	size_t getEmployeeID() {return employee_ID;}

private:
	static size_t  sn;
	std::string employee_name;
	size_t employee_ID;
};

size_t Employee::sn = 1;

int main() {
	
	Employee a, b(string("baokun")), c;
	cout << a.getEmployeeName() << endl;
	cout << a.getEmployeeID() << endl;

	cout << b.getEmployeeName() << endl;
	cout << b.getEmployeeID() << endl;

	cout << c.getEmployeeName() << endl;
	cout << c.getEmployeeID() << endl;
}
