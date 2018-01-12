#include "atoms.h"

namespace gologpp {
namespace readylog {


EC_ref Variable::translate()
{ return ec_var; }


EC_atom Value::translate()
{ return ec_atom; }


}
}