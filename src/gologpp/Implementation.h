#ifndef GOLOGPP_IMPLEMENTATION_H_
#define GOLOGPP_IMPLEMENTATION_H_

namespace gologpp {

namespace generic {
class ExecutionContext;
}


class AbstractImplementation {
public:
	virtual ~AbstractImplementation() = default;
	virtual void apply_to(generic::ExecutionContext &) = 0;
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