#ifndef READYLOG_ACTION_H_
#define READYLOG_ACTION_H_

#include <gologpp/Action.h>
#include <gologpp/utilities.h>
#include <gologpp/Translation.h>
#include <eclipseclass.h>

#include <tuple>

namespace gologpp {
namespace readylog {


class Action : public Translatable<generic::Action, tuple<EC_word, EC_word, EC_word>> {
public:
	Action(generic::Action &&action);
    
private:
	void init(tuple<EC_word, EC_word, EC_word> &cache);
};


} // namespace readylog
} // namespace gologpp


#endif