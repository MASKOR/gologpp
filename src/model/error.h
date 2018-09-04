#ifndef GOLOGPP_ERROR_H_
#define GOLOGPP_ERROR_H_

#include "gologpp.h"

#include <exception>
#include <string>

namespace gologpp {

class InconsistentModel : public std::exception {
};

class EngineError : public std::exception {
};

class Bug : public std::exception {
public:
	Bug(const std::string &msg);
	virtual ~Bug() override = default;

	virtual const char *what() const noexcept override;

private:
	std::string msg_;
};


class UserError : public std::exception {
};

class ExpressionTypeMismatch : public UserError {
public:
	ExpressionTypeMismatch(const Expression &expr1_, const Expression &expr2_);

private:
	const Expression &expr1_;
	const Expression &expr2_;
};

}



#endif /* GOLOGPP_ERROR_H_ */
