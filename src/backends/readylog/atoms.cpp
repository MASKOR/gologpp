#include "atoms.h"

namespace gologpp {
namespace readylog {


void Variable::init(EC_word &cache)
{ cache = ::newvar(); }

}
}