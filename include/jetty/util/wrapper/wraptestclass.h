#ifndef __WRAPPERTESTCLASS__HH
#define __WRAPPERTESTCLASS__HH

#include <iostream>

class WrapTestClass
{
public:
	WrapTestClass() : i(counter++) {;}
	WrapTestClass(const WrapTestClass &t) : i (t.i) {;}
	virtual ~WrapTestClass()
	{
		std::cout << "debug: ---> WrapTestClass::~WrapTestClass() " << i << " at 0x" << this << std::endl;
	}
	void test_call()
	{
		std::cout << "debug: ---> WrapTestClass test_call i = " << i << std::endl;
	}
	static int counter;
private:
	int i;
};

#endif // __WRAPPERTESTCLASS__HH
