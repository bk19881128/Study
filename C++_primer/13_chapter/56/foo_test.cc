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

class Foo {
public:
	Foo () = default;
	Foo (const vector<int> &s) : data(s) {};
	Foo sorted() &&;
	Foo sorted() const &;
	Foo test_sorted1() const &;
	Foo test_sorted2() const &;
	void print_data();
private:
	vector<int> data;
};

Foo Foo::sorted() &&
{
	cout << "call sorted &&" << endl;
	sort(data.begin(), data.end());
	return *this;
}

Foo Foo::sorted() const & 
{
	cout << "call sorted const &" << endl;
	Foo ret(*this);
	sort(ret.data.begin(), ret.data.end());
	return ret;
}

void Foo::print_data()
{
	for(auto iter = data.begin(); iter != data.end(); iter++)
		cout << *iter << " ";
	cout << endl;
}

Foo Foo::test_sorted1() const & {
	Foo ret(*this);
	return ret.sorted();
}

Foo Foo::test_sorted2() const & {
	return Foo(*this).sorted();
}

Foo retval() {
        vector<int> v = {3, 2, 4, 1};
        Foo foo_obj(v);
	return foo_obj;
}

int main() {

	vector<int> v = {3, 2, 4, 1};
	Foo foo_obj(v);

	foo_obj.sorted();
	foo_obj.print_data();
	retval().sorted().print_data();

	foo_obj.test_sorted1();
	foo_obj.test_sorted2();

	return 0;
}
