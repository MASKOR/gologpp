#ifndef GOLOGPP_SITUATION_H
#define GOLOGPP_SITUATION_H

#include "Execution.h"
#include "Fluent.h"

#include <unordered_map>

namespace gologpp {
namespace generic {

using namespace std;

class Situation {
public:
	Situation();

protected:
	unordered_map<shared_ptr<Fluent>, unique_ptr<AnyValue>> fluent_values_;
};


} // namespace generic
} // namespace gologpp

#endif // GOLOGPP_SITUATION_H