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

template<class... Ts>
using tuple = std::tuple<Ts...>;

using string = std::string;


namespace generic {

template<class action_impl, class fluent_impl, class proc_impl>
struct ImplConfig {
	typedef action_impl action_impl_t;
	typedef fluent_impl fluent_impl_t;
	typedef proc_impl proc_impl_t;
};

} // namespace generic


} // namespace gologpp

#endif // GOLOGPP_H_