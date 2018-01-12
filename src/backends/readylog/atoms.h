#ifndef READYLOG_ATOMS_H_
#define READYLOG_ATOMS_H_

#include <gologpp/atoms.h>
#include <gologpp/Translation.h>
#include <eclipseclass.h>

namespace gologpp {
namespace readylog {


class Variable : public Translatable<gologpp::Variable, EC_ref> {
public:
	Variable(gologpp::Variable &&var);
	virtual EC_ref translate() override;

private:
	EC_ref ec_var;
};


class Value : public Translatable<gologpp::Value, EC_atom> {
public:
	using Translatable<gologpp::Value, EC_atom>::Translatable;
	virtual EC_atom translate() override;

private:
	EC_atom ec_atom;
};


} // namespace readylog
} // namespace gologpp



#endif