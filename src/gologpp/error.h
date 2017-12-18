/*
 * error.h
 *
 *  Created on: 03.05.2016
 *      Author: ich
 */

#ifndef GOLOGPP_ERROR_H_
#define GOLOGPP_ERROR_H_

#include <exception>
#include <string>

namespace gologpp {

using namespace std;

class InconsistentModel : public std::exception {
};

class EngineError : public std::exception {
};

class Bug : public std::exception {
public:
	Bug(const string &msg)
	    : msg_(msg) {}

	virtual const char *what() const noexcept override
	{ return msg_.c_str(); }

private:
	string msg_;
};

}



#endif /* GOLOGPP_ERROR_H_ */
