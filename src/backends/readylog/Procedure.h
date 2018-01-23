#ifndef READYLOG_PROCEDURE_H_
#define READYLOG_PROCEDURE_H_

#include <gologpp/Procedure.h>
#include <gologpp/Translation.h>
#include <eclipseclass.h>

namespace gologpp {
namespace readylog {

using namespace std;


class Block : public Translatable<generic::Block, EC_word> {
};


class Choose : public Translatable<generic::Choose, EC_word> {
};


class Conditional : public Translatable<generic::Conditional, EC_word> {
};


} // namespace readylog
} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
