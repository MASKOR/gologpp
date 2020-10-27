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

#include "mapping.h"
#include "error.h"
#include "action.h"
#include "utilities.h"
#include "reference.h"

#include <boost/fusion/include/at_c.hpp>


namespace gologpp {

BackendMapping::BackendMapping(
	const string &backend_name,
	boost::optional<vector<fusion_wtf_vector<string, Expression *>>> arg_mapping
)
: backend_name_(backend_name)
{
	for (auto &pair : arg_mapping.get_value_or({}))
		arg_mapping_.emplace(
			boost::fusion::at_c<0>(pair),
			unique_ptr<Expression>(boost::fusion::at_c<1>(pair))
		);
}

BackendMapping::BackendMapping(const Global &identity)
: backend_name_(identity.name())
{
	for (const shared_ptr<Variable> &param : identity.params())
		arg_mapping_.emplace(param->name(), param->ref());
}


const string &BackendMapping::backend_name() const
{ return backend_name_; }


const Expression &BackendMapping::mapped_expr(const string &name) const
{
	auto it = arg_mapping_.find(name);
	if (it == arg_mapping_.end())
		throw NameNotMapped("Missing mapping entry for backend argument name '" + name + "`");

	return *it->second;
}

shared_ptr<const Variable> BackendMapping::mapped_var(const string &name) const
{
	try {
		return mapped_expr(name).cast<Reference<Variable>>().target();
	} catch (std::bad_cast &) {
		return nullptr;
	}
}

bool BackendMapping::is_mapped(const string &arg_name) const
{ return arg_mapping_.find(arg_name) != arg_mapping_.end(); }


void BackendMapping::attach_semantics(SemanticsFactory &f)
{
	for (auto &pair : arg_mapping_)
		pair.second->attach_semantics(f);
}

const BackendMapping::ArgMapping &BackendMapping::arg_mapping() const
{ return arg_mapping_; }


string BackendMapping::to_string(const string &pfx) const
{
	string rv = linesep + pfx + '"' + backend_name_ + "\" {" linesep;
	for (auto &pair : arg_mapping_)
		rv += pfx + indent + pair.first + " = " + pair.second->str() + "," linesep;

	if (arg_mapping_.size() > 0)
		rv = rv.substr(0, rv.size() - 2);

	return rv + linesep + pfx + "}" linesep;
}



}
