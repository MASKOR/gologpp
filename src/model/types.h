#ifndef GOLOGPP_COMPOUND_TYPE_H_
#define GOLOGPP_COMPOUND_TYPE_H_

#include "gologpp.h"
#include "user_error.h"
#include "utilities.h"

#include <unordered_map>

namespace gologpp {


void ensure_type_equality(const AbstractLanguageElement &e1, const AbstractLanguageElement &e2);


class Type
: public std::enable_shared_from_this<Type>
, public Name {
public:
	virtual ~Type() = default;

	virtual bool operator == (const Type &other) const;
	virtual bool operator == (const string &type_name) const;
	bool operator != (const Type &other) const;
	bool operator != (const string &type_name) const;
	virtual operator bool () const;

	virtual bool is_compound() const;
	virtual bool is_simple() const;

	template<class T>
	bool is() const;

	void ensure_match(const AbstractLanguageElement &e) const;

protected:
	Type(const string &name);
};


template<class T>
bool Type::is() const
{ return typeid(*this) == typeid(T); }



class UndefinedType : public Type {
public:
	UndefinedType();

	virtual operator bool () const override;
	virtual bool is_simple() const override;
	static string name();
};



class BoolType : public Type {
public:
	BoolType();
	static string name();
};



class NumberType : public Type {
public:
	NumberType();
	static string name();
};



class StringType : public Type {
public:
	StringType();
	static string name();
};



class SymbolType : public Type {
public:
	SymbolType();
	static string name();
};



class VoidType : public Type {
public:
	VoidType();
	static string name();
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
			throw NoSuchFieldError(field_name, name());
		return dynamic_cast<const T &>(*(it->second));
	}

	template<class T>
	bool has_type(const string &field_name)
	{ return std::dynamic_pointer_cast<T>(field_type(field_name)); }

	bool has_field(const string &name) const;
	void add_field(const string &name, const string &type);

	bool has_field_of_type(const string &field_name, const string &type_name) const;

	static string name();

	virtual bool operator == (const Type &other) const override;
	virtual bool operator == (const string &type_name) const override;

	virtual bool is_compound() const override;

private:
	std::unordered_map<string, shared_ptr<const Type>> fields_;
};



}

#endif // GOLOGPP_COMPOUND_TYPE_H_
