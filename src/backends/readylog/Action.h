#ifndef READYLOG_ACTION_H_
#define READYLOG_ACTION_H_

#include <gologpp/Action.h>
#include <gologpp/utilities.h>
#include <gologpp/Translation.h>
#include <eclipseclass.h>

#include <tuple>

namespace gologpp {
namespace readylog {


class Action : public gologpp::Translatable<gologpp::Action, tuple<EC_word, EC_word, EC_word>> {
public:
	Action(gologpp::Action &&action);
    
    virtual tuple<EC_word, EC_word, EC_word> translate() override;

private:
	tuple<EC_word, EC_word, EC_word> cache_;
};


} // namespace readylog
} // namespace gologpp


#endif