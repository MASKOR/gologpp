#include "types.h"
#include "scope.h"

namespace gologpp {


void ensure_type_equality(const AbstractLanguageElement &e1, const AbstractLanguageElement &e2)
{
	if (e1.type() != e2.type())
		throw ExpressionTypeMismatch(e1, e2);
}



string to_string(Type::Tag t)
{
	switch (t) {
	case Type::Tag::BOOLEAN:
		return "?";
	case Type::Tag::NUMERIC:
		return "%";
	case Type::Tag::SYMBOLIC:
		return "§";
	case Type::Tag::STRING:
		return "$";
	case Type::Tag::VOID:
		return "";
	case Type::Tag::COMPOUND:
		return "@";
	}
	return "[Unknown ExpressionTypeTag]";
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

bool Type::is_compound() const
{ return false; }

bool Type::is_simple() const
{ return !is_compound(); }

void Type::ensure_match(const AbstractLanguageElement &e) const
{
	if (e.type() == *this)
		throw ExpressionTypeMismatch(e.str() + " does not match type " + name());
}




Bool::Bool()
: Type(static_name())
{}

Type::Tag Bool::tag()
{ return Type::Tag::BOOLEAN; }

Type::Tag Bool::dynamic_tag() const
{ return tag(); }

string Bool::static_name()
{ return "bool"; }



Number::Number()
: Type(static_name())
{}

Type::Tag Number::tag()
{ return Type::Tag::NUMERIC; }

Type::Tag Number::dynamic_tag() const
{ return tag(); }

string Number::static_name()
{ return "number"; }



String::String()
: Type(static_name())
{}

Type::Tag String::tag()
{ return Type::Tag::STRING; }

Type::Tag String::dynamic_tag() const
{ return tag(); }

string String::static_name()
{ return "string"; }



Symbol::Symbol()
: Type(static_name())
{}

Type::Tag Symbol::tag()
{ return Type::Tag::SYMBOLIC; }

Type::Tag Symbol::dynamic_tag() const
{ return tag(); }

string Symbol::static_name()
{ return "symbol"; }



Void::Void()
: Type(static_name())
{}

Type::Tag Void::tag()
{ return Type::Tag::VOID; }

Type::Tag Void::dynamic_tag() const
{ return tag(); }

string Void::static_name()
{ return "void"; }



CompoundType::CompoundType(const string &name)
: Type(name)
{}

Type::Tag CompoundType::tag()
{ return Type::Tag::COMPOUND; }

Type::Tag CompoundType::dynamic_tag() const
{ return tag(); }

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



