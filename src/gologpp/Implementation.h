#ifndef GOLOGPP_IMPLEMENTATION_H_
#define GOLOGPP_IMPLEMENTATION_H_

namespace gologpp {


class AbstractImplementation {
public:
	virtual ~AbstractImplementation() = default;
};


template<class GologT>
class Implementation : public AbstractImplementation {
public:
	Implementation(const GologT &obj)
	: golog_obj_(obj)
	{}

	virtual ~Implementation();

private:
	const GologT &golog_obj_;
};


}

#endif // GOLOGPP_IMPLEMENTATION_H_
