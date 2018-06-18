#ifndef GOLOGPP_UTILITIES_H_
#define GOLOGPP_UTILITIES_H_

#include "gologpp.h"
#include <string>


namespace gologpp {

class Name {
public:
    Name(const string &name);
    Name(Name &&other);
    Name(const Name &other) = default;
    virtual ~Name() = default;
    
    const string &name() const;
    virtual bool operator == (const Name &other) const;
    virtual size_t hash() const;
    
private:
    const string name_;
};


class Identifier : public Name {
public:
    Identifier(const string &name, arity_t arity);
    Identifier(Identifier &&other);
    Identifier(const Identifier &other) = default;
    
    virtual ~Identifier() override = default;

    arity_t arity() const;

    virtual bool operator == (const Identifier &other) const;
    virtual size_t hash() const override;

private:
    const arity_t arity_;
};


struct identifier_hash {
	template<class IdPtrT>
	std::size_t operator () (const IdPtrT &p) const {
		return std::dynamic_pointer_cast<Identifier>(p)->hash();
	}
};


struct identifier_equals {
	template<class IdPtrT>
	bool operator () (const IdPtrT &lhs, const IdPtrT &rhs) const {
		return *std::dynamic_pointer_cast<Identifier>(lhs) == *std::dynamic_pointer_cast<Identifier>(rhs);
	}
};


} // namespace gologpp



namespace std {

template<>
struct hash<gologpp::Identifier> {
    size_t operator () (const gologpp::Identifier &o) const
    { return o.hash(); }
};

} // namespace std

#endif // GOLOGPP_UTILITIES_H_
