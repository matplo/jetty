#ifndef __WRAPTYPE__HH
#define __WRAPTYPE__HH

#include <string>
#include <vector>
#include <memory>
#include <typeindex>

#include <iostream>

class WrapType
{
public:
	WrapType()
	: fName("no name")
	, fID(0)
	, fHashCode(0)
	{
		;
	}

	virtual ~WrapType() 
	{
		;
	}

	void set_name(const char *name)
	{
		fName = name;
	}

	void set_hashcode(size_t hash)
	{
		fHashCode = hash;
	}

	void set_id(unsigned int uid)
	{
		fID = uid;
	}

	void set_type_name(const char *tname)
	{
		fTypeName = tname;
	}

	unsigned int get_id() const
	{
		return fID;
	}

	std::string get_name() const
	{
		return fName;
	}

	size_t get_hashcode() const
	{
		return fHashCode;
	}

	std::string get_type_name() const 
	{
		return fTypeName;
	}

	friend std::ostream& operator <<(std::ostream& out, const WrapType& c)
	{
		out << "> @ 0x" << &c << " type name: " << std::type_index(typeid(c)).name() << std::endl
			<< "    name:     " << c.get_name() << std::endl
			<< "    type:     " << c.get_type_name() << std::endl
			<< "    hashcode: " << c.get_hashcode() << std::endl
			<< "    id:       " << c.get_id() << std::endl;
		return out;
	} 

protected:
	std::string 		fName;
	std::string         fTypeName;
	unsigned int		fID;
	size_t				fHashCode;
};

#endif // __WRAPTYPE__HH
