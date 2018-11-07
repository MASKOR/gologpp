#include "history.h"

namespace gologpp {



HistorySemantics::HistorySemantics(History &h)
: history_(h)
{}

HistorySemantics::~HistorySemantics()
{}



History::History()
{}

Scope &History::parent_scope()
{ return global_scope(); }

const Scope &History::parent_scope() const
{ return global_scope(); }


string History::to_string(const string &pfx) const
{ return pfx + linesep + "history: no representation" + linesep; }



} // namespace gologpp