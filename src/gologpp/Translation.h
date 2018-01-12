#ifndef GOLOGPP_TRANSLATION_H_
#define GOLOGPP_TRANSLATION_H_

#include <tuple>
#include <utility>

namespace gologpp {


template<class GologT, class TargetT>
class Translatable : public GologT {
public:
	using GologT::GologT;

	Translatable(GologT &&base)
	: GologT(std::forward<GologT>(base))
	{}

	virtual TargetT translate() = 0;
};



}

#endif // GOLOGPP_TRANSLATION_H_