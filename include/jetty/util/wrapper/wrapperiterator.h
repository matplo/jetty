#ifndef __WRAPPERIERATOR__HH
#define __WRAPPERIERATOR__HH

#include <string>
#include <vector>
#include <memory>
#include <typeindex>

#include <iostream>

#include "wrapper.h"

template <class T>
class WrapperIterator
{
public:
	WrapperIterator (const Wrapper *w, bool forward = true)
	: fWrapper(w)
	, fForward(forward)
	, fPos (0)
	, fIndexes()
	{
		reset(forward);
	}

	void reset(bool forward = true)
	{
		fForward = forward;
		fIndexes.clear();
		unsigned int isize = fWrapper->size();
		for (unsigned int i = 0; i < isize; i++)
		{
			if (fWrapper->get_at<T>(i) != 0x0)
				fIndexes.push_back(i);
		}
		if (fForward == false)
		{
			fPos = fIndexes.size();
		}
		else
		{
			fPos = 1;
		}
	}

	~WrapperIterator() {;}

	T *next()
	{
		T *p = 0;
		if (fPos == 0)
		{
			return p;
		}
		if (fPos - 1 < fIndexes.size())
		{
			p = fWrapper->get_at<T>(fPos - 1);
			if (fForward == false)
			{
				fPos--;
			}
			else
			{
				fPos++;
			}
		}
		return p;
	}

private:
	const Wrapper *fWrapper;
	bool fForward;
	unsigned int fPos;
	std::vector<unsigned int> fIndexes;
};

#endif // __WRAPPERITERATOR__HH
