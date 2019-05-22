#include "variable.h"
#include "reference.h"

namespace gologpp {


Variable::Variable(const string &type_name, const string &name)
: Identifier(name, 0)
, domain_(new Domain(type_name))
{
	set_type_by_name(type_name);
	domain_->add_subject(*this);
}


Reference<Variable> *Variable::ref()
{
	return new Reference<Variable>(
		this->shared_from_this()
	);
}

Domain &Variable::domain()
{ return *domain_; }

const Domain &Variable::domain() const
{ return *domain_; }


void Variable::add_implicit_domain_element(const Constant &c)
{
	if (!domain_->is_implicit())
		return;
	domain_->elements().emplace(new Constant(
		dynamic_cast<const Constant &>(c)
	) );
}

void Variable::set_domain(const string &domain_name)
{
	domain_ = global_scope().lookup_domain(domain_name);
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
	global_scope().register_domain(new Domain(type().name(), domain_name, {}, true));
	set_domain(domain_name);
}

string Variable::to_string(const string &) const
{ return name(); }


}
