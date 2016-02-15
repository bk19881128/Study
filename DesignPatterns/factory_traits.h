#ifndef _BKFACTORYTRAITS_H_ 
#define _BKFACTORYTRAITS_H_
#include <stdio.h>

namespace BKLIB{

#define CREATE(type) create##type()

struct Dummy {};


template <typename PRODUCT1, typename PRODUCT2, typename PRODUCT3, typename PRODUCT4, typename PRODUCT5,typename PRODUCT6, 
	typename PRODUCT7, typename PRODUCT8, typename PRODUCT9, typename PRODUCT10>
struct FactoryTraits
{ 
public: 
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	typedef PRODUCT2 Product2;
	typedef PRODUCT3 Product3;
	typedef PRODUCT4 Product4;
	typedef PRODUCT5 Product5;
	typedef PRODUCT6 Product6;
	typedef PRODUCT7 Product7;
	typedef PRODUCT8 Product8;
	typedef PRODUCT9 Product9;
	typedef PRODUCT10 Product10;

	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}
	virtual PRODUCT2* CREATE(Product2)
 	{
		return new PRODUCT2();
	}
	virtual PRODUCT3* CREATE(Product3)
	{
		return new PRODUCT3();
	}
	virtual PRODUCT4* CREATE(Product4)
	{
		return new PRODUCT4();
	}
	virtual PRODUCT5* CREATE(Product5)
	{
		return new PRODUCT5();
	}
	virtual PRODUCT6* CREATE(Product6)
	{
		return new PRODUCT6();
	}
	virtual PRODUCT7* CREATE(Product7)
	{
		return new PRODUCT7();
	}
	virtual PRODUCT8* CREATE(Product8)
	{
		return new PRODUCT8();
	}
	virtual PRODUCT9* CREATE(Product9)
	{
		return new PRODUCT9();
	}
	virtual PRODUCT10* CREATE(Product10)
	{
		return new PRODUCT10();
	}


	FactoryTraits()
	{}
private:

};

template<typename PRODUCT1, typename PRODUCT2, typename PRODUCT3, typename PRODUCT4, typename PRODUCT5,
	typename PRODUCT6, typename PRODUCT7, typename PRODUCT8, typename PRODUCT9>
struct FactoryTraits<PRODUCT1, PRODUCT2, PRODUCT3, PRODUCT4, PRODUCT5,
                   PRODUCT6, PRODUCT7, PRODUCT8, PRODUCT9, BKLIB::Dummy> 
{ 
public: 
	FactoryTraits(){}
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	typedef PRODUCT2 Product2;
	typedef PRODUCT3 Product3;
	typedef PRODUCT4 Product4;
	typedef PRODUCT5 Product5;
	typedef PRODUCT6 Product6;
	typedef PRODUCT7 Product7;
	typedef PRODUCT8 Product8;
	typedef PRODUCT9 Product9;

	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}
	virtual PRODUCT2* CREATE(Product2)
	{
		return new PRODUCT2();
	}
	virtual PRODUCT3* CREATE(Product3)
	{
		return new PRODUCT3();
	}
	virtual PRODUCT4* CREATE(Product4)
	{
		return new PRODUCT4();
	}
	virtual PRODUCT5* CREATE(Product5)
	{
		return new PRODUCT5();
	}
	virtual PRODUCT6* CREATE(Product6)
	{
		return new PRODUCT6();
	}
	virtual PRODUCT7* CREATE(Product7)
	{
		return new PRODUCT7();
	}
	virtual PRODUCT8* CREATE(Product8)
	{
		return new PRODUCT8();
	}
	virtual PRODUCT9* CREATE(Product9)
	{
		return new PRODUCT9();
	}

private:

};


template <typename PRODUCT1, typename PRODUCT2, typename PRODUCT3, typename PRODUCT4, typename PRODUCT5,
	typename PRODUCT6, typename PRODUCT7, typename PRODUCT8>
struct FactoryTraits<PRODUCT1, PRODUCT2, PRODUCT3, PRODUCT4, PRODUCT5,
                   PRODUCT6, PRODUCT7, PRODUCT8, BKLIB::Dummy, BKLIB::Dummy> 
{ 
public: 
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	typedef PRODUCT2 Product2;
	typedef PRODUCT3 Product3;
	typedef PRODUCT4 Product4;
	typedef PRODUCT5 Product5;
	typedef PRODUCT6 Product6;
	typedef PRODUCT7 Product7;
	typedef PRODUCT8 Product8;

	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}
	virtual PRODUCT2* CREATE(Product2)
	{
		return new PRODUCT2();
	}
	virtual PRODUCT3* CREATE(Product3)
	{
		return new PRODUCT3();
	}
	virtual PRODUCT4* CREATE(Product4)
	{
		return new PRODUCT4();
	}
	virtual PRODUCT5* CREATE(Product5)
	{
		return new PRODUCT5();
	}
	virtual PRODUCT6* CREATE(Product6)
	{
		return new PRODUCT6();
	}
	virtual PRODUCT7* CREATE(Product7)
	{
		return new PRODUCT7();
	}
	virtual PRODUCT8* CREATE(Product8)
	{
		return new PRODUCT8();
	}


	FactoryTraits(){}
private:

};

template <typename PRODUCT1, typename PRODUCT2, typename PRODUCT3, typename PRODUCT4, typename PRODUCT5,
	typename PRODUCT6, typename PRODUCT7>
struct FactoryTraits<PRODUCT1, PRODUCT2, PRODUCT3, PRODUCT4, PRODUCT5,
                   PRODUCT6, PRODUCT7, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy> 
{ 
public: 
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	typedef PRODUCT2 Product2;
	typedef PRODUCT3 Product3;
	typedef PRODUCT4 Product4;
	typedef PRODUCT5 Product5;
	typedef PRODUCT6 Product6;
	typedef PRODUCT7 Product7;

	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}
	virtual PRODUCT2* CREATE(Product2)
	{
		return new PRODUCT2();
	}
	virtual PRODUCT3* CREATE(Product3)
	{
		return new PRODUCT3();
	}
	virtual PRODUCT4* CREATE(Product4)
	{
		return new PRODUCT4();
	}
	virtual PRODUCT5* CREATE(Product5)
	{
		return new PRODUCT5();
	}
	virtual PRODUCT6* CREATE(Product6)
	{
		return new PRODUCT6();
	}
	virtual PRODUCT7* CREATE(Product7)
	{
		return new PRODUCT7();
	}


	FactoryTraits(){}
private:

};

template <typename PRODUCT1, typename PRODUCT2, typename PRODUCT3, typename PRODUCT4, typename PRODUCT5,
	typename PRODUCT6>
struct FactoryTraits<PRODUCT1, PRODUCT2, PRODUCT3, PRODUCT4, PRODUCT5,
                   PRODUCT6, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy> 
{ 
public: 
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	typedef PRODUCT2 Product2;
	typedef PRODUCT3 Product3;
	typedef PRODUCT4 Product4;
	typedef PRODUCT5 Product5;
	typedef PRODUCT6 Product6;

	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}
	virtual PRODUCT2* CREATE(Product2)
	{
		return new PRODUCT2();
	}
	virtual PRODUCT3* CREATE(Product3)
	{
		return new PRODUCT3();
	}
	virtual PRODUCT4* CREATE(Product4)
	{
		return new PRODUCT4();
	}
	virtual PRODUCT5* CREATE(Product5)
	{
		return new PRODUCT5();
	}
	virtual PRODUCT6* CREATE(Product6)
	{
		return new PRODUCT6();
	}



	FactoryTraits(){}
private:

};

template <typename PRODUCT1, typename PRODUCT2, typename PRODUCT3, typename PRODUCT4, typename PRODUCT5>
struct FactoryTraits<PRODUCT1, PRODUCT2, PRODUCT3, PRODUCT4, PRODUCT5,
                   BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy> 
{ 
public: 
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	typedef PRODUCT2 Product2;
	typedef PRODUCT3 Product3;
	typedef PRODUCT4 Product4;
	typedef PRODUCT5 Product5;

	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}
	virtual PRODUCT2* CREATE(Product2)
	{
		return new PRODUCT2();
	}
	virtual PRODUCT3* CREATE(Product3)
	{
		return new PRODUCT3();
	}
	virtual PRODUCT4* CREATE(Product4)
	{
		return new PRODUCT4();
	}
	virtual PRODUCT5* CREATE(Product5)
	{
		return new PRODUCT5();
	}



	FactoryTraits(){
		printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$five\n");
	}
private:

};

template <typename PRODUCT1, typename PRODUCT2, typename PRODUCT3, typename PRODUCT4>
struct FactoryTraits<PRODUCT1, PRODUCT2, PRODUCT3, PRODUCT4, BKLIB::Dummy,
                   BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy> 
{ 
public: 
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	typedef PRODUCT2 Product2;
	typedef PRODUCT3 Product3;
	typedef PRODUCT4 Product4;

	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}
	virtual PRODUCT2* CREATE(Product2)
	{
		return new PRODUCT2();
	}
	virtual PRODUCT3* CREATE(Product3)
	{
		return new PRODUCT3();
	}
	virtual PRODUCT4* CREATE(Product4)
	{
		return new PRODUCT4();
	}

	FactoryTraits(){}
private:

};

template <typename PRODUCT1, typename PRODUCT2, typename PRODUCT3>
struct FactoryTraits<PRODUCT1, PRODUCT2, PRODUCT3, BKLIB::Dummy, BKLIB::Dummy,
                   BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy> 
{ 
public: 
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	typedef PRODUCT2 Product2;
	typedef PRODUCT3 Product3;

	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}
 	virtual PRODUCT2* CREATE(Product2)
   	{
		return new PRODUCT2();
	}
	virtual PRODUCT3* CREATE(Product3)
	{
		return new PRODUCT3();
	}

	FactoryTraits(){}
private:

};

template <typename PRODUCT1, typename PRODUCT2>
struct FactoryTraits<PRODUCT1, PRODUCT2, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy,
                   BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy> 
{ 
public: 
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	typedef PRODUCT2 Product2;

	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}
	virtual PRODUCT2* CREATE(Product2)
	{
		return new PRODUCT2();
	}

	FactoryTraits(){}
private:

};

template <typename PRODUCT1>
struct FactoryTraits<PRODUCT1, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy,
                   BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy, BKLIB::Dummy> 
{ 
public: 
	FactoryTraits(){
	}
	virtual ~FactoryTraits(){}

	typedef PRODUCT1 Product1;
	
	virtual PRODUCT1* CREATE(Product1)
	{
		return new PRODUCT1();
	}

private:

};


template <typename PRODUCT1=BKLIB::Dummy, typename PRODUCT2=BKLIB::Dummy, typename PRODUCT3=BKLIB::Dummy,
	typename PRODUCT4=BKLIB::Dummy, typename PRODUCT5=BKLIB::Dummy,typename PRODUCT6=BKLIB::Dummy, typename PRODUCT7=BKLIB::Dummy, 
	typename PRODUCT8=BKLIB::Dummy, typename PRODUCT9=BKLIB::Dummy, typename PRODUCT10=BKLIB::Dummy>
struct Factory: FactoryTraits<PRODUCT1, PRODUCT2, PRODUCT3, PRODUCT4, PRODUCT5,
                   PRODUCT6, PRODUCT7, PRODUCT8, PRODUCT9, PRODUCT10> 
{ 
public: 
	Factory(){}

	virtual ~Factory(){}

	
private:

};

}//BKLIB

#endif
