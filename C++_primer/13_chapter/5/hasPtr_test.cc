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

class HasPtr {
public:
	HasPtr(const std::string &s = std::string()):
		ps(new std::string(s)), i(0) { }
	
	HasPtr(const HasPtr &p):
		ps(new std::string(*p.ps)), i(p.i) { }

	HasPtr& operator=(const HasPtr &);

	~HasPtr() { delete ps; }

	int getInt() { return i; }
	string getString() { return *ps; }

private:
	std::string *ps;
	int i;
};

HasPtr& HasPtr::operator=(const HasPtr &rhs)
{
	string *newp = new string(*rhs.ps);
	delete ps;
	ps = newp;
	i = rhs.i;
	return *this;
}

int main()
{
	HasPtr h1("good");
	cout << h1.getInt() << endl;
	cout << h1.getString() << endl;

	HasPtr h2(h1);
	cout << h2.getInt() << endl;
	cout << h2.getString() << endl;

	HasPtr h3;
	h3 = h1;
	cout << h3.getInt() << endl;
	cout << h3.getString() << endl;

	return 0;
}	
