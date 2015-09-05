#include "Person_data.h"

int main()
{
	cout << "to test Person_data class" << endl;
	cout << "construct data1" << endl;
	Person_data data1;
	cout << "construct data2" << endl;
	Person_data data2("baokun","123456");
	cout << "construct data3" << endl;
	Person_data data3(cin);

	cout << "read data1" << endl;
	read(cin, data1);
	print(cout, data1);
	cout << endl;
	cout << data1.getname() << endl;
	cout << data1.getaddress() << endl;

        print(cout, data2);
        cout << endl;
        cout << data2.getname() << endl;
        cout << data2.getaddress() << endl;

        print(cout, data3);
        cout << endl;
        cout << data3.getname() << endl;
        cout << data3.getaddress() << endl;

}
