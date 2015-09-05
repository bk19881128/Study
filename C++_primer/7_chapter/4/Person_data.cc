#include "Person_data.h"

Person_data::Person_data() {

}

Person_data::Person_data(istream &is) {
	read(is, *this);
}

istream &read(istream& is, Person_data& person) {
	is >> person.name >> person.address;
	return is;
}

ostream &print(ostream& os, const Person_data& person) {
	os << person.name << " " << person.address;
	return os;
}
