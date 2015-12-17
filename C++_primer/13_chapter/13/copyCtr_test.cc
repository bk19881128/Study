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

class X {
public:
	X(const std::string &s = std::string()):
		ps(new std::string(s)) { std::cout << "X(const string)" << std::endl; }
	X(const X &p):
		ps(new std::string(*p.ps)) { std::cout << "X(const X&)" << std::endl; }
	X& operator=(const X&);
	~X() { std::cout << "~X()" << std::endl; delete ps;}

	string getString() {return *ps;}

private:
	std::string *ps;
};

X& X::operator=(const X &rhs)
{
	std::cout << "X::operator=" << std::endl;
	string *newp = new string(*rhs.ps);
	delete ps;
	ps = newp;
	return *this;
}

void print_X(X s) 
{
	cout << s.getString() << " print_X" << endl;
}

void print_rX(X& s)
{
	cout << s.getString() << " print_rX" << endl;
}

int main()
{
	X x1("good");

	X x2(x1);
	X x3;
	x3 = x2;

	X* x4 = new X();	
	*x4 = x1;

	print_X(x1);
	print_rX(x1);

	cout << "in container:" << endl;
	vector<X> v1;
	return 0;
}	
