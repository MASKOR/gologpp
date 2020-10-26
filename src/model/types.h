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

#ifndef GOLOGPP_COMPOUND_TYPE_H_
#define GOLOGPP_COMPOUND_TYPE_H_

#include "gologpp.h"
#include "error.h"
#include "name.h"

#include <unordered_map>
#include <unordered_set>

namespace gologpp {



class Type
: public std::enable_shared_from_this<Type> {
public:
	using ElementType = Type;

	friend class Scope;

	Type(const Type &) = delete;
	Type(Type &&) = delete;

	virtual ~Type() = default;

	virtual bool operator >= (const Type &other) const;
	virtual bool operator >= (const AbstractLanguageElement &e) const;
	virtual bool operator <= (const Type &other) const;
	bool operator == (const Type &) const = delete;
	bool operator != (const Type &) const = delete;
	virtual operator bool () const;

	virtual bool is_compound() const;
	virtual bool is_simple() const;

	template<class T>
	bool is() const;

	string name() const;
	string mangled_name() const;
	static string demangle_name(const string &s);
	explicit operator string () const;

protected:
	Type(const string &name);
	Name name_;
	static const string mangle_pfx_;
};


using PType = shared_ptr<const Type>;


template<class T>
bool Type::is() const
{ return typeid(*this) == typeid(T); }



class UndefinedType : public Type {
public:
	UndefinedType();

	virtual bool operator >= (const Type &other) const override;
	virtual bool operator >= (const AbstractLanguageElement &e) const override;
	virtual bool operator <= (const Type &other) const override;
	virtual operator bool () const override;
	virtual bool is_simple() const override;
	static string static_name();
};



class BoolType : public Type {
public:
	BoolType();
	static string static_name();
};



class NumberType : public Type {
public:
	NumberType();
	static string static_name();
};



class StringType : public Type {
public:
	StringType();
	static string static_name();
};



class SymbolType : public Type {
public:
	SymbolType();
	static string static_name();
};



class VoidType : public Type {
public:
	VoidType();
	static string static_name();
};



class CompoundType : public Type {
public:
	using Representation = std::unordered_map<string, unique_ptr<Value>>;

	CompoundType(const string &name);

	template<class T = Type>
	const T &field_type(const string &field_name) const
	{
		auto it = fields_.find(field_name);
		if (it == fields_.end())
			throw NoSuchFieldError(field_name, static_cast<string>(*this));
		return dynamic_cast<const T &>(*(it->second));
	}

	template<class T>
	bool has_type(const string &field_name)
	{ return std::dynamic_pointer_cast<T>(field_type(field_name)); }

	bool has_field(const string &name) const;
	void add_field(const string &name, const Type &type);

	bool has_field_of_type(const string &field_name, const Type &type) const;
	std::unordered_set<string> field_names() const;

	virtual bool operator >= (const Type &other) const override;

	virtual bool is_compound() const override;

	static string static_name();

private:
	std::unordered_map<string, shared_ptr<const Type>> fields_;
};



class ListType : public Type {
public:
	using Representation = vector<unique_ptr<Value>>;

	ListType(const Type &elem_type);
	ListType(const string &elem_type_name);

	const Type &element_type() const;
	PType element_type_ptr() const;

	virtual bool operator >= (const Type &other) const override;
	virtual bool operator >= (const AbstractLanguageElement &) const override;

	virtual bool is_compound() const override;
	virtual bool is_simple() const override;

	static string static_name();

private:
	const Type &elem_type_;
};


}

#endif // GOLOGPP_COMPOUND_TYPE_H_
