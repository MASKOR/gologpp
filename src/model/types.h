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
	bool operator != (const Type &other) const;

	virtual bool is_compound() const;
	virtual bool is_simple() const;

	void ensure_match(const AbstractLanguageElement &e) const;

protected:
	Type(const string &name);
};



class UndefinedType : public Type {
public:
	UndefinedType();

	virtual bool operator == (const Type &other) const override;
	virtual operator bool () const override;

	virtual bool is_simple() const override;

	static string static_name();
};



class Bool : public Type {
public:
	Bool();
	static string static_name();
};



class Number : public Type {
public:
	Number();
	static string static_name();
};



class String : public Type {
public:
	String();
	static string static_name();
};



class Symbol : public Type {
public:
	Symbol();
	static string static_name();
};



class Void : public Type {
public:
	Void();
	static string static_name();
};



class CompoundType : public Type {
public:
	using Representation = std::unordered_map<string, unique_ptr<Constant>>;

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

	bool has_field(const string &name);
	void add_field(const string &name, const string &type);

	static string static_name();

	virtual bool operator == (const Type &other) const override;

	virtual bool is_compound() const override;

private:
	std::unordered_map<string, shared_ptr<Type>> fields_;
};





}

#endif // GOLOGPP_COMPOUND_TYPE_H_
