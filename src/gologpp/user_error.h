#ifndef GOLOGPP_USER_ERROR_H
#define GOLOGPP_USER_ERROR_H

#include <string>
#include <exception>


namespace gologpp {

using namespace std;


class SemanticError : public std::exception {
public:
	SemanticError(const string &description);
	
	virtual const char *what() const noexcept override;
	
private:
	string message_;
};



} // namespace gologpp







#endif // GOLOGPP_USER_ERROR_H
