#include "factory_traits.h"

using namespace BKLIB;

class pro{
public:
	pro()
	{
		printf("pro was created!\n");
	}

};

int main() {

	printf(">>>>>>>>>>>>>>>>>>>>>>Factory Traits>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

	Factory<pro> factory;
	factory.createProduct1();
	Factory<pro,pro,pro,pro,pro> factory5;
	factory5.createProduct1();
	factory5.createProduct4();
}


