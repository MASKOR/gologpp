#ifndef GOLOGPP_ERROR_H_
#define GOLOGPP_ERROR_H_

#include "gologpp.h"

#include <exception>
#include <string>

namespace gologpp {

class InconsistentModel : public std::exception {
};

class EngineError : public std::runtime_error {
	using std::runtime_error::runtime_error;
};



class Bug : public std::exception {
public:
	Bug(const std::string &msg);
	virtual ~Bug() override = default;

	virtual const char *what() const noexcept override;

private:
	std::string msg_;
};


class DuplicateTransition : public EngineError {
	using EngineError::EngineError;
};

class LostTransition : public EngineError {
	using EngineError::EngineError;
};

class InconsistentTransition : public EngineError {
	using EngineError::EngineError;
};



class UserError : public std::runtime_error {
public:
	UserError(const string &msg);
};



class ExpressionTypeMismatch : public UserError {
public:
	ExpressionTypeMismatch(const Expression &expr1_, const Expression &expr2_);
};

}



#endif /* GOLOGPP_ERROR_H_ */
