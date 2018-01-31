#ifndef READYLOG_EXECUTION_H_
#define READYLOG_EXECUTION_H_

#include <gologpp/Execution.h>
#include <eclipseclass.h>

namespace gologpp {
namespace readylog {

using namespace std;

class ExecutionContext : public generic::ExecutionContext {
public:
    virtual ~ExecutionContext() override;
    static ExecutionContext &instance();

    void compile(const EC_word &term);

private:
    ExecutionContext();

};


class EclipseError : public std::exception {
	using std::exception::exception;
};

} // namespace readylog
} // namespace gologpp

#endif // READYLOG_EXECUTION_H_