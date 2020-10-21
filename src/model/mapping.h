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

#ifndef GOLOGPP_MAPPING_H_
#define GOLOGPP_MAPPING_H_


#include "utilities.h"
#include "expressions.h"
#include "scope.h"

#include <unordered_map>
#include <boost/fusion/include/vector.hpp>
#include <boost/optional.hpp>

namespace gologpp {


template<class T1, class T2>
#ifdef BOOST_FUSION_HAS_VARIADIC_VECTOR
using fusion_wtf_vector = boost::fusion::vector<T1, T2>;
#else
using fusion_wtf_vector = boost::fusion::vector2<T1, T2>;
#endif


/** @class BackendMapping Maps golog++ expressions (typically action arguments) to the parameter names in a backend action.
 * For an @ref ExogAction, the mapped golog++ expressions must be the action's arguments.
 * For an endogenous @ref Action, arbitrary golog++ expressions can be used.
 */
class BackendMapping
: public LanguageElement<BackendMapping>
, public NoScopeOwner
, public ChildElement
{
public:
	using ArgMapping = std::unordered_map<
		string,
		unique_ptr<Expression>
	>;

	BackendMapping(
		const string &backend_name,
		boost::optional<vector<fusion_wtf_vector<string, Expression *>>> arg_mapping
	);

	/// Create a default (identity) mapping
	BackendMapping(const Global &identity);

	~BackendMapping() = default;

	const string &backend_name() const;

	/// @param backend_param_name Name of the data field in the PlatformBackend implementation
	/// @return The expression mapped to @ref backend_param_name.
	/// Throws an @ref UserError if nothing is mapped to @ref backend_param_name.
	const Expression &mapped_expr(const string &backend_param_name) const;

	/// @param backend_param_name Name of the data field in the PlatformBackend implementation
	/// @return The action parameter mapped to @ref backend_param_name. nullptr if the mapped @ref Expression is not a @ref Variable.
	/// Throws an @ref UserError if nothing is mapped to @ref backend_param_name
	shared_ptr<const Variable> mapped_var(const string &backend_param_name) const;

	/// @param backend_param_name Name of the data field in the PlatformBackend implementation
	/// @return whether any golog++ @ref Expression is mapped to @ref backend_param_name
	bool is_mapped(const string &backend_param_name) const;

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;

	const ArgMapping &arg_mapping() const;

private:
	string backend_name_;
	ArgMapping arg_mapping_;
};


}


#endif // GOLOGPP_MAPPING_H_
