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

#include "types.h"
#include "scope.h"
#include "domain.h"
#include "list.h"
#include "logger.h"

namespace gologpp {


const string Type::mangle_pfx_("gpptype~");


Type::Type(const string &name)
: name_(name)
{}

bool Type::operator >= (const Type &other) const
{
	if (other.is<Domain>()) {
		try {
			const Domain &d = dynamic_cast<const Domain &>(other);
			return *this >= d.element_type();
		} catch (std::bad_cast &) {
			return false;
		}
	}
	else
		return this == &other || name() == other.name() || other.is<UndefinedType>();
}

bool Type::operator >= (const AbstractLanguageElement &e) const
{ return *this >= e.type(); }

bool Type::operator <= (const Type &other) const
{ return other >= *this; }

Type::operator bool () const
{ return true; }

bool Type::is_compound() const
{ return false; }

bool Type::is_simple() const
{ return !is_compound(); }

string Type::name() const
{ return name_; }

string Type::mangled_name() const
{ return mangle_pfx_ + name(); }


string Type::demangle_name(const string &s)
{
	if (s.substr(0, mangle_pfx_.length()) != mangle_pfx_) {
		log(LogLevel::ERR) << __func__ << ": name is not mangled: " + s << flush;
		return s;
	}
	else
		return s.substr(mangle_pfx_.length());
}

Type::operator string () const
{ return name_; }



UndefinedType::UndefinedType()
: Type(static_name())
{}

bool UndefinedType::operator >= (const Type &) const
{ return true; }

bool UndefinedType::operator >= (const AbstractLanguageElement &) const
{ return true; }

bool UndefinedType::operator <= (const Type &) const
{ return true; }

bool UndefinedType::is_simple() const
{ return false; }

UndefinedType::operator bool () const
{ return false; }

string UndefinedType::static_name()
{ return "~undefined~"; }




BoolType::BoolType()
: Type(static_name())
{}

string BoolType::static_name()
{ return "bool"; }



NumberType::NumberType()
: Type(static_name())
{}

string NumberType::static_name()
{ return "number"; }



StringType::StringType()
: Type(static_name())
{}

string StringType::static_name()
{ return "string"; }



SymbolType::SymbolType()
: Type(static_name())
{}

string SymbolType::static_name()
{ return "symbol"; }



VoidType::VoidType()
: Type(static_name())
{}

string VoidType::static_name()
{ return "void"; }



CompoundType::CompoundType(const string &name)
: Type(name)
{}

string CompoundType::static_name()
{ return "compound"; }


void CompoundType::add_field(const string &name, const Type &type)
{
	if (has_field(name))
		throw UserError("compound type `" + this->name() + "': duplicate field name: `" + name + "'");

	fields_.insert({name, type.shared_from_this()});
}

bool CompoundType::has_field_of_type(const string &field_name, const Type &type) const
{ return has_field(field_name) && field_type(field_name) >= type; }

bool CompoundType::has_field(const string &name) const
{ return fields_.find(name) != fields_.end(); }


std::unordered_set<string> CompoundType::field_names() const
{
	std::unordered_set<string> rv;
	for (const auto &pair : fields_)
		rv.insert(pair.first);
	return rv;
}


bool CompoundType::operator >= (const Type &other) const
{
	if (other.is<UndefinedType>())
		return true;
	try {
		const CompoundType &o = dynamic_cast<const CompoundType &>(other);
		for (auto pair : fields_) {
			if (!(o.field_type(pair.first) >= *pair.second))
				return false;
		}
		return true;
	} catch (std::bad_cast &) {
		return false;
	}
}


bool CompoundType::is_compound() const
{ return true; }



ListType::ListType(const Type &elem_type)
: Type("list[" + elem_type.name() + "]")
, elem_type_(elem_type)
{}

ListType::ListType(const string &elem_type_name)
: ListType(*global_scope().lookup_type<Type>(elem_type_name))
{}

string ListType::static_name()
{ return "list"; }

const Type &ListType::element_type() const
{ return elem_type_; }

PType ListType::element_type_ptr() const
{ return elem_type_.shared_from_this(); }

bool ListType::operator >= (const Type &other) const
{
	if (other.is<UndefinedType>())
		return true;
	try {
		const ListType &t = dynamic_cast<const ListType &>(other);
		return this->element_type() >= t.element_type();
	} catch (std::bad_cast &) {
		return false;
	}
}

bool ListType::operator >= (const AbstractLanguageElement &other) const
{
	if (*this >= other.type())
		return true;
	else if (other.is_a<Value>())
		return dynamic_cast<const Value &>(other) <= *this;
	else if (other.is_a<ListExpression>())
		return dynamic_cast<const ListExpression &>(other) <= *this;
	else
		return false;
}

bool ListType::is_compound() const
{ return false; }

bool ListType::is_simple() const
{ return false; }



}



