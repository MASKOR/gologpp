#ifndef GOLOGPP_USER_ERROR_H
#define GOLOGPP_USER_ERROR_H

#include <string>
#include <exception>


namespace gologpp {


class SemanticError : public std::exception {
public:
	SemanticError(const std::string &description);
	
	virtual const char *what() const noexcept override;
	
private:
	std::string message_;
};



} // namespace gologpp







#endif // GOLOGPP_USER_ERROR_H
