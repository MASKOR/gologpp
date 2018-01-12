#ifndef READYLOG_EXECUTION_H_
#define READYLOG_EXECUTION_H_

#include <gologpp/Execution.h>

namespace gologpp {
namespace readylog {

using namespace std;

class ExecutionContext : private gologpp::ExecutionContext {
public:
    virtual ~ExecutionContext();
    static ExecutionContext &instance();
    
private:
    ExecutionContext();
    
};

} // namespace readylog
} // namespace gologpp

#endif // READYLOG_EXECUTION_H_