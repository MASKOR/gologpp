#include "utilities.h"

namespace gologpp {
namespace parser {


template<class ExprT>
struct FieldAccessParser : public grammar<FieldAccess<ExprT> *(Scope &)> {
	FieldAccessParser();

	rule<FieldAccess<ExprT> *(Scope &)> field_access;
};


} // namespace parser
} // namespace gologpp
