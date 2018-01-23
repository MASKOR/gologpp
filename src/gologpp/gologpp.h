#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <memory>

namespace gologpp {

typedef unsigned char arity_t;


class AbstractImplementation {

};


template<class ImplT>
class Implementation : public std::enable_shared_from_this<Implementation<ImplT>>, public AbstractImplementation {
public:
	Implementation();
};




}

#endif // GOLOGPP_H_