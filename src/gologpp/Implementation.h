#ifndef GOLOGPP_IMPLEMENTATION_H_
#define GOLOGPP_IMPLEMENTATION_H_

namespace gologpp {

namespace generic {
class AbstractExecutionContext;
}

class AbstractImplementation {
public:
	virtual ~AbstractImplementation() = default;

	template<class exec_ctx_t>
	void init(exec_ctx_t &);
};


template<class GologT>
class Implementation; /*: public AbstractImplementation {
public:
	Implementation(const GologT &obj)
	: golog_obj_(obj)
	{}

	virtual ~Implementation() = 0;

private:
	const GologT &golog_obj_;
};*/


}

#endif // GOLOGPP_IMPLEMENTATION_H_
