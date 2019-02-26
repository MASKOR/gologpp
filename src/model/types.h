#ifndef GOLOGPP_COMPOUND_TYPE_H_
#define GOLOGPP_COMPOUND_TYPE_H_

#include "gologpp.h"
#include "user_error.h"
#include "utilities.h"

#include <unordered_map>

namespace gologpp {


class Type
: public std::enable_shared_from_this<Type>
, public Name {
public:
	enum Tag {
		BOOLEAN,
		NUMERIC,
		SYMBOLIC,
		STRING,
		VOID,
		COMPOUND
	};

	virtual ~Type() = default;

	virtual bool operator == (const Type &other) const;
	bool operator != (const Type &other) const;

	virtual bool is_compound() const;
	bool is_simple() const;

	virtual Tag dynamic_tag() const = 0;

protected:
	Type(const string &name);
};


// TODO: Remove
using ExpressionTypeTag = Type::Tag;


string to_string(Type::Tag t);



class Bool : public Type {
public:
	Bool();

	static Tag tag();
	virtual Tag dynamic_tag() const override;
	static string static_name();
	static const Bool &instance();
};



class Number : public Type {
public:
	Number();

	static Tag tag();
	virtual Tag dynamic_tag() const override;
	static string static_name();
	static const Number &instance();
};



class String : public Type {
public:
	String();

	static Tag tag();
	virtual Tag dynamic_tag() const override;
	static string static_name();
	static const String &instance();
};



class Symbol : public Type {
public:
	Symbol();

	static Tag tag();
	virtual Tag dynamic_tag() const override;
	static string static_name();
	static const Symbol &instance();
};



class Void : public Type {
public:
	Void();

	static Tag tag();
	virtual Tag dynamic_tag() const override;
	static string static_name();
	static const Void &instance();
};



class CompoundType : public Type {
public:
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

	static Tag tag();
	virtual Tag dynamic_tag() const override;
	static string static_name();

	virtual bool operator == (const Type &other) const override;

	virtual bool is_compound() const override;

private:
	std::unordered_map<string, shared_ptr<Type>> fields_;
};




}

#endif // GOLOGPP_COMPOUND_TYPE_H_
