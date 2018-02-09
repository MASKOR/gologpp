#ifndef READYLOG_UTILITIES_H_
#define READYLOG_UTILITIES_H_

#include <eclipseclass.h>
#include <vector>

#include "Implementation.h"

namespace gologpp {

template<class ElemT>
EC_word *compile_list(const std::vector<ElemT> &list)
{
	EC_word *rv = new EC_word[list.size()];
	std::size_t i = 0;
	for (const ElemT &elem : list)
		rv[i++] = dynamic_cast<ReadylogExpression &>(elem->implementation()).term();
	return rv;
}

}

#endif // READYLOG_UTILITIES_H_