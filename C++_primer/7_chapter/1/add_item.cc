/*
 * This file contains code from "C++ Primer, Fifth Edition", by Stanley B.
 * Lippman, Josee Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2013 by Objectwrite, Inc., Josee Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Permissions Department
 * 	One Lake Street
 * 	Upper Saddle River, NJ  07458
 * 	Fax: (201) 236-3290
*/ 

#include <iostream>
using namespace std;

#include "Sales_data.h"

bool readData(Sales_data& data) {
	string i_bookNo;
	unsigned i_units_sold;
	double i_revenue;

	cout << "please input a record" << endl;
	cout << "bookNo:" << endl;	
	if (!(cin >> i_bookNo))
		return false;
	cout << "units_sold:" << endl;
        if (!(cin >> i_units_sold))
                return false;
	cout << "revenue:" << endl;
        if (!(cin >> i_revenue))
                return false;

	data.bookNo = i_bookNo;
	data.units_sold = i_units_sold;
	data.revenue = i_revenue; 

	return true;
}

void updataData(Sales_data& data1, Sales_data data2) {
	data1.units_sold += data2.units_sold;
	data1.revenue += data2.revenue;
}

void printData(Sales_data data) {
	cout << data.bookNo;
        cout << " " << data.units_sold;
	cout << " " << data.revenue << endl;
}

int main()
{
	Sales_data total;
	if (readData(total)) {
		Sales_data trans;
		while(readData(trans)) {
			if (total.bookNo == trans.bookNo)
				updataData(total, trans);
			else {
				printData(total);
				total = trans;
			}
		}
		printData(total);
	}
	else {
		cerr << "No data?!" << endl;
		return -1;
	}

	return 0;
}	
