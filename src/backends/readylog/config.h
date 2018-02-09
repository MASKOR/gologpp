#ifndef READYLOG_CONFIG_H_
#define READYLOG_CONFIG_H_

#include <gologpp/gologpp.h>

#include <backends/readylog/Action.h>
#include <backends/readylog/Fluent.h>
#include <backends/readylog/Procedure.h>

namespace gologpp {


namespace generic {
template<class impl_config_t>
class ExecutionContext;
}


template<class GologT>
class Implementation;


typedef generic::ImplConfig<
	Implementation<generic::Action>,
	Implementation<generic::Fluent>,
	Implementation<generic::Procedure>
> readylog_impl_config_t;


} // namespace gologpp


#endif // READYLOG_CONFIG_H_