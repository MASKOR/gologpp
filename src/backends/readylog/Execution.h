#ifndef READYLOG_EXECUTION_H_
#define READYLOG_EXECUTION_H_

#include <gologpp/Execution.h>
#include "config.h"
#include <eclipseclass.h>

namespace gologpp {


class EclipseContext : public ExecutionContext
{
public:
    virtual ~EclipseContext();
    static EclipseContext &instance();

    void compile(const EC_word &term);

private:
    EclipseContext();

};


class EclipseError : public std::exception {
	using std::exception::exception;
};


} // namespace gologpp

#endif // READYLOG_EXECUTION_H_
