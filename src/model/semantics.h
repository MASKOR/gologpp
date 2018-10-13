#ifndef GOLOGPP_IMPLEMENTATION_H_
#define GOLOGPP_IMPLEMENTATION_H_

#include "gologpp.h"
#include <memory>

#include <boost/preprocessor/seq/for_each.hpp>

namespace gologpp {


template<class GologT>
class Semantics;


class AbstractSemantics {
public:
	virtual ~AbstractSemantics() = default;
};


#define GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS(r, data, T) \
	virtual unique_ptr<AbstractSemantics> make_semantics(T &) = 0;

class SemanticsFactory {
public:
	virtual ~SemanticsFactory() = default;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS, (), GOLOGPP_SEMANTIC_TYPES)
};


}

#endif // GOLOGPP_IMPLEMENTATION_H_
