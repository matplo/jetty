#ifndef __WRAPPER__HH
#define __WRAPPER__HH

#include <string>
#include <vector>
#include <memory>
#include <typeindex>

#include <iostream>

#include "wraptype.h"
#include "wrapcontainer.h"

class Wrapper
{
	public:
		Wrapper()
		: fPointers()
		, idcount(0)
		, debug(0)
		{
			;
		}

		virtual 		~Wrapper()
		{
			for (unsigned int i = 0; i < fPointers.size(); i++)
			{
				if (debug) 
					{
						std::cout << "[d] Deleting... " << fPointers[i]->get_name() << std::endl;
						std::cout << *fPointers[i] << std::endl;
					}

				delete fPointers[i];
			}
		}

		template <class T>
		unsigned int 	add(T *p)
		{
			if (contains(p))
			{
				std::string name = std::type_index(typeid(p)).name();
				throw std::runtime_error(std::string("adding same pointer twice (?) typeid name: ") + name);
			}
			unsigned int id = idcount;
			idcount++;
			std::string name = std::type_index(typeid(p)).name();
			WrapContainer<T> *c = new WrapContainer<T>(p, id, name.c_str());
			c->take_ownership();
			fPointers.push_back(c);
			return id;
		}

		template <class T>
		unsigned int 	add(const T &o)
		{
			T *p = new T(o);
			return add(p);
		}

		template <class T>
		bool contains(T *p)
		{
			for (unsigned int i = 0; i < fPointers.size(); i++)
			{
				WrapContainer<T> *c = (WrapContainer<T>*)(fPointers[i]); // always returns c!=0
				if (c->get() == p)
					return true;
			}
			return false;
		}

		template <class T>
		bool remove(T *p)
		{
			for (unsigned int i = 0; i < fPointers.size(); i++)
			{
				WrapContainer<T> *c = (WrapContainer<T>*)(fPointers[i]); // always returns c!=0
				if (c->get() == p)
					{
						delete fPointers[i];
						fPointers.erase(fPointers.begin() + i);
						return true;
					}
			}
			return false;			
		}

		template <class T>
		T* get() const
		{
			T *p = 0x0;
			size_t tmphash = std::type_index(typeid(p)).hash_code();
			for (unsigned int i = 0; i < fPointers.size(); i++)
			{
				WrapContainer<T> *c = (WrapContainer<T>*)(fPointers[i]); // always returns c!=0
				if (c->HasHash(tmphash))
				{
					p = c->get();
				}
			}
			return p;
		}

		template <class T>
		T* get_unchecked(unsigned int iwhich) const
		{
			T *p = 0x0;
			if ( iwhich < fPointers.size())
			{
				WrapContainer<T> *c = (WrapContainer<T>*)(fPointers[iwhich]); // always returns c!=0
				p = c->get();
			}
			return p;
		}

		template <class T>
		T* get(unsigned int iwhich) const 
		{
			T *p = 0x0;
			if ( iwhich < fPointers.size() )
			{
				size_t tmphash = std::type_index(typeid(p)).hash_code();
				WrapContainer<T> *c = (WrapContainer<T>*)(fPointers[iwhich]); // always returns c!=0
				if (c->HasHash(tmphash))
					p = c->get();
			}
			return p;
		}

		template <class T>
		long index()
		{
			long idx = -1;
			T *p = 0x0;
			size_t tmphash = std::type_index(typeid(p)).hash_code();
			for (unsigned int i = 0; i < fPointers.size(); i++)
			{
				WrapContainer<T> *c = (WrapContainer<T>*)(fPointers[i]); // always returns c!=0
				if (c->HasHash(tmphash))
				{
					idx = i;
				}
			}
			return idx;
		}

		template <class T>
		void remove_all()
		{
			T *p = 0x0;
			long idx = index<T>();
			while (idx >= 0)
			{
				delete fPointers[idx];
				fPointers.erase(fPointers.begin() + idx);
				idx = index<T>();
			}
		}

		void list()
		{
			std::cout << "[i] Wrapper::list() ..." << std::endl << std::endl;
			for (unsigned int i = 0; i < fPointers.size(); i++)
			{
				// this is a trick - we do not need a template argument here
				WrapContainer<bool> *c = (WrapContainer<bool>*)(fPointers[i]); // always returns c!=0
				WrapContainer<bool> &rc = *c;
				std::cout << rc << std::endl;
			}			
		}

		unsigned int size() const
		{
			return fPointers.size();
		}

		void set_debug(unsigned int idbg)
		{
			debug = idbg;
		}

	protected:

		std::vector< WrapType* >		fPointers;
		//std::forward_list< WrapType* >		fPointers;

	private:
		unsigned int 	idcount;
		unsigned int	debug;
};

#endif // __WRAPPER__HH
