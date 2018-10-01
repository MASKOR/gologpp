#ifndef READYLOG_IMPLEMENTATION_H_
#define READYLOG_IMPLEMENTATION_H_

#include <model/semantics.h>

#include <eclipseclass.h>

#include <boost/preprocessor/seq/for_each.hpp>

namespace gologpp {


template<>
class Semantics<AbstractLanguageElement> : public AbstractSemantics {
public:
	virtual ~Semantics() override = default;
	virtual EC_word plterm() = 0;
};


using ReadylogSemantics = Semantics<AbstractLanguageElement>;


#define GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE(_r, _data, GologT) \
	virtual unique_ptr<AbstractSemantics> make_semantics(GologT &) override;


class ReadylogSemanticsFactory : public SemanticsFactory {
public:
	ReadylogSemanticsFactory() = default;
	virtual ~ReadylogSemanticsFactory() override = default;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE, (), GOLOGPP_SEMANTIC_TYPES)
};


}

#endif
