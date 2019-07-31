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
{ return this == &other || typeid(*this) == typeid(other); }

bool Type::operator == (const string &type_name) const
{ return name() == type_name; }

bool Type::operator != (const Type &other) const
{ return !(*this == other); }

bool Type::operator != (const string &type_name) const
{ return !(*this == type_name); }

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
: Type(name())
{}

bool UndefinedType::is_simple() const
{ return false; }

UndefinedType::operator bool () const
{ return false; }

string UndefinedType::name()
{ return "~undefined~"; }




BoolType::BoolType()
: Type(name())
{}

string BoolType::name()
{ return "bool"; }



NumberType::NumberType()
: Type(name())
{}

string NumberType::name()
{ return "number"; }



StringType::StringType()
: Type(name())
{}

string StringType::name()
{ return "string"; }



SymbolType::SymbolType()
: Type(name())
{}

string SymbolType::name()
{ return "symbol"; }



VoidType::VoidType()
: Type(name())
{}

string VoidType::name()
{ return "void"; }



CompoundType::CompoundType(const string &name)
: Type(name)
{}

string CompoundType::name()
{ return "compound"; }


void CompoundType::add_field(const string &name, const string &type)
{
	if (has_field(name))
		throw UserError("compound type `" + this->name() + "': duplicate field name: `" + name + "'");

	fields_.insert({name, global_scope().lookup_type(type)});
}

bool CompoundType::has_field_of_type(const string &field_name, const string &type_name) const
{ return has_field(field_name) && field_type(field_name).name() == type_name; }

bool CompoundType::has_field(const string &name) const
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

bool CompoundType::operator == (const string &type_name) const
{ return name() == type_name || type_name == CompoundType::name(); }

bool CompoundType::is_compound() const
{ return true; }



ListType::ListType(const Type &elem_type)
: Type(elem_type.name() + "[]")
, elem_type_(elem_type.shared_from_this())
{}

string ListType::name()
{ return "list"; }

const Type &ListType::element_type() const
{ return *elem_type_; }

bool ListType::is_compound() const
{ return false; }

bool ListType::is_simple() const
{ return false; }



}



