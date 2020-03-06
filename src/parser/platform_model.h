#ifndef GOLOGPP_PARSER_PLATFORM_MODEL_H_
#define GOLOGPP_PARSER_PLATFORM_MODEL_H_

#include "utilities.h"

#include <model/platform_model.h>

namespace gologpp {
namespace parser {

struct PlatformParser : public grammar<PlatformModel *()> {
	PlatformParser();

private:
	rule<PlatformState()> state;
	rule<vector<PlatformState>()> states;
	rule<vector<PlatformTransition>()> transitions;
	rule<PlatformModel *()> platform;
	rule<Duration()> duration;
	rule<PlatformTransition()> transition;
	rule<std::string()> text;
};

} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_PLATFORM_MODEL_H_
