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

#include "variable.h"
#include "reference.h"

namespace gologpp {


Variable::Variable(const Type &t, const string &name)
: Identifier(name, 0)
{
	set_type(t);

	if (t.is<Domain>())
		domain_ = global_scope().lookup_domain(t.name(), t);
	else
		domain_.reset(new Domain(t));

	domain_->add_subject(*this);
}


Reference<Variable> *Variable::ref()
{
	return new Reference<Variable>(
		std::dynamic_pointer_cast<Variable>(this->shared_from_this())
	);
}

Domain &Variable::domain()
{ return *domain_; }

const Domain &Variable::domain() const
{ return *domain_; }


void Variable::add_implicit_domain_element(const Value &c)
{
	if (!domain_->is_implicit())
		return;
	domain_->elements().emplace(new Value(
		dynamic_cast<const Value &>(c)
	) );
}

void Variable::set_domain(const string &domain_name)
{
	domain_ = global_scope().lookup_domain(domain_name, type());
	domain_->add_subject(*this);
}

void Variable::set_domain_copy(const Domain &domain)
{
	if (domain_ && domain_->is_defined())
		throw std::runtime_error("domain already set for " + name());
	domain_ = std::make_shared<Domain>(
		"anonymous_domain(" + str() + ")",
		domain
	);
	domain_->add_subject(*this);
}

void Variable::define_implicit_domain(const string &domain_name)
{
	global_scope().register_domain_raw(new Domain(domain_name, type(), {}, true));
	set_domain(domain_name);
}

string Variable::to_string(const string &) const
{ return name(); }


}
