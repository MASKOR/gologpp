#ifndef READYLOG_REFERENCE_H_
#define READYLOG_REFERENCE_H_

#include "Implementation.h"
#include <eclipseclass.h>


namespace gologpp {

namespace generic {
template<class GologT> class Reference;
} // namespace generic


template<class GologT>
class Implementation<generic::Reference<GologT>> : public ReadylogExpression {

};




} // namespace gologpp



#endif // READYLOG_REFERENCE_H_