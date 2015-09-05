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

#ifndef PERSON_DATA_H
#define PERSON_DATA_H

#include <string>
#include <iostream>

using namespace std;

class Person_data {

friend ostream &print(ostream&, const Person_data&);
friend istream &read(istream&, Person_data&);

public:
	// constructors
	Person_data();
	Person_data(const string &s, const string &n):
		    name(s), address(n) { }
	Person_data(istream &is);

	// operations on Person_data objects
	string getname() const { return name; }
	string getaddress() const { return address; }
private:
	string name;
	string address;
};

// nonmember Person_data interface functions
ostream &print(ostream&, const Person_data&);
istream &read(istream&, Person_data&);

#endif
