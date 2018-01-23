#ifndef GOLOGPP_TRANSLATION_H_
#define GOLOGPP_TRANSLATION_H_

#include <tuple>
#include <utility>
#include "utilities.h"

namespace gologpp {


template<class GologT, class TargetT>
class Translatable : public GologT {
public:
	using GologT::GologT;

	typedef Translatable<GologT, TargetT> translatable;

	Translatable(GologT &&base)
	: GologT(std::forward<GologT>(base))
	{}

	virtual TargetT translate() const
	{ return cache_; }

	virtual void init_translation() {
		init(cache_);
	}

private:
	virtual void init(TargetT &cache) = 0;
	TargetT cache_;
};


} // namespace gologpp

#endif // GOLOGPP_TRANSLATION_H_