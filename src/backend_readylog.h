#ifndef GOLOGPP_IMPLEMENTATION_H_
#define GOLOGPP_IMPLEMENTATION_H_

namespace gologpp {


class AbstractImplementation {

};


template<class GologT>
class Implementation : public AbstractImplementation {
public:
	Implementation(const GologT &obj);

	virtual ~Implementation()
	{}
};


}

#endif // GOLOGPP_IMPLEMENTATION_H_