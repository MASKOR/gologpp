#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <memory>

namespace gologpp {

typedef unsigned char arity_t;

template<class T>
using shp = std::shared_ptr<T>;

template<class T>
using shared_ptr = std::shared_ptr<T>;

template<class T>
using uqp = std::unique_ptr<T>;

template<class T>
using unique_ptr = std::unique_ptr<T>;

using string = std::string;



}

#endif // GOLOGPP_H_