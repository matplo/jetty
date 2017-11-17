#ifndef __WRAPCONTAINER__HH
#define __WRAPCONTAINER__HH

#include <string>
#include <vector>
#include <memory>
#include <typeindex>

#include <iostream>

#include "wraptype.h"

template <class T> class WrapContainer : public WrapType
{
public:
	WrapContainer(T* p, unsigned int id = 0, const char *name = 0)
	: WrapType()
	, fUP(p)
	, fpU(0)
	, fIdx(typeid(p))
	{
		set_hashcode( fIdx.hash_code() );
		if (name != 0)
			set_name(name);
		else
			set_name( fIdx.name() );
		set_id(id);
		set_type_name(fIdx.name());
	}

	WrapContainer(const WrapContainer<T> &c)
	: WrapType()
	, fUP(c.fUP)
	, fpU(0)
	, fIdx(c.fIdx)
	{
		set_name(c.fName.c_str());
		set_id(c.fID);
		set_hashcode(c.fHashCode);

		take_ownership();
	}

	void take_ownership()
	{
		if (fpU != 0)
		{
			delete fpU;
			fpU = 0;
		}
		fpU   = new std::unique_ptr<T>(fUP);
	}

	virtual ~WrapContainer()
	{
		self_delete();
	}

	void self_delete()
	{
		fpU->reset();
		delete fpU;
	}

	T* get(unsigned int i) const
	{
		if (i == get_id())
			return fUP;
		return 0x0;
	}

	T* get() const
	{
		return fUP;
	}

	bool IsNamed(const char *name)
	{
		if (fName == name)
			return true;
		return false;
	}

	template <class X>
	bool HasHash(X* p)
	{
		size_t tmphash = std::type_index(typeid(p)).hash_code();
		//std::cout << "- comparing: " << fHashCode << " : " << tmphash << std::endl;
		return get_hashcode() == tmphash;
	}

	bool HasHash(size_t tmphash)
	{
		return get_hashcode() == tmphash;
	}

private:
	WrapContainer() {;}
	T*				 	fUP;
	std::unique_ptr<T> 	*fpU;
	std::type_index     fIdx;
};

#endif // __WRAPCONTAINER__HH
