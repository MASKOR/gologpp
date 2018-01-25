#ifndef READYLOG_REFERENCE_H_
#define READYLOG_REFERENCE_H_

#include "Implementation.h"
#include <eclipseclass.h>


namespace gologpp {

namespace generic {
template<class GologT> class Reference;
class FluentReference;
class Fluent;
} // namespace generic


template<class GologT>
class Implementation<generic::Reference<GologT>> : public AbstractImplementation {

};


template<>
class Implementation<generic::FluentReference> : public ReadylogExpression {

};


} // namespace gologpp



#endif // READYLOG_REFERENCE_H_