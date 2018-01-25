#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <memory>

namespace gologpp {

typedef unsigned char arity_t;

template<class T>
using shp = std::shared_ptr<T>;

template<class T>
using uqp = std::unique_ptr<T>;



}

#endif // GOLOGPP_H_