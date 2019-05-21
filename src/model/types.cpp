#include "types.h"
#include "scope.h"

namespace gologpp {


void ensure_type_equality(const AbstractLanguageElement &e1, const AbstractLanguageElement &e2)
{
	if (e1.type() != e2.type())
		throw ExpressionTypeMismatch(e1, e2);
}


Type::Type(const string &name)
: Name(name)
{}

bool Type::operator == (const Type &other) const
{
	if (this == &other)
		return true;
	return typeid(*this) == typeid(other);
}

bool Type::operator != (const Type &other) const
{ return !(*this == other); }

Type::operator bool () const
{ return true; }

bool Type::is_compound() const
{ return false; }

bool Type::is_simple() const
{ return !is_compound(); }

void Type::ensure_match(const AbstractLanguageElement &e) const
{
	if (e.type() == *this)
		throw ExpressionTypeMismatch(e.str() + " does not match type " + name());
}



UndefinedType::UndefinedType()
: Type(static_name())
{}

bool UndefinedType::operator == (const Type &) const
{ return false; }

bool UndefinedType::is_simple() const
{ return false; }

UndefinedType::operator bool () const
{ return false; }

string UndefinedType::static_name()
{ return "~undefined~"; }




Bool::Bool()
: Type(static_name())
{}

string Bool::static_name()
{ return "bool"; }



Number::Number()
: Type(static_name())
{}

string Number::static_name()
{ return "number"; }



String::String()
: Type(static_name())
{}

string String::static_name()
{ return "string"; }



Symbol::Symbol()
: Type(static_name())
{}

string Symbol::static_name()
{ return "symbol"; }



Void::Void()
: Type(static_name())
{}

string Void::static_name()
{ return "void"; }



CompoundType::CompoundType(const string &name)
: Type(name)
{}

string CompoundType::static_name()
{ return "compound"; }


void CompoundType::add_field(const string &name, const string &type)
{
	if (has_field(name))
		throw UserError("compound type `" + this->name() + "': duplicate field name: `" + name + "'");

	fields_.insert({name, global_scope().lookup_type(type)});
}


bool CompoundType::has_field(const string &name)
{ return fields_.find(name) != fields_.end(); }


bool CompoundType::operator == (const Type &other) const
{
	try {
		const CompoundType o = dynamic_cast<const CompoundType &>(other);
		for (auto pair : fields_) {
			if (o.field_type(pair.first) != *pair.second)
				return false;
		}
		return true;
	} catch (std::bad_cast &) {
		return false;
	}
}

bool CompoundType::is_compound() const
{ return true; }


}



