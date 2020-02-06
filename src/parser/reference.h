/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef GOLOGPP_PARSER_REFERENCE_H_
#define GOLOGPP_PARSER_REFERENCE_H_

#include "utilities.h"


namespace gologpp {
namespace parser {


/******************
* References
******************/

template<class GologT>
struct ReferenceParser : grammar<Reference<GologT> *(Scope &, const Type &)> {
	ReferenceParser();

	rule<Reference<GologT> *(Scope &, const Type &)> start;
	rule<Reference<GologT> *(Scope &, const Type &), locals<TypeList>> pred_ref;
	rule<vector<Expression *>(Scope &, TypeList), locals<TypeList>> ref_args;
};


template<class GologT>
rule<Reference<GologT> *(Scope &, const Type &)> &typed_reference()
{
	static ReferenceParser<GologT> rp;
	static rule<Reference<GologT> *(Scope &, const Type &)> rv { rp(_r1, _r2) };
	return rv;
}


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_REFERENCE_H_

