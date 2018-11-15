#include "mapping.h"

namespace gologpp{

ActionMapping::ActionMapping(const string &name, const vector<Expression *> &args)
: Identifier (name, arity_t(args.size()))
, args_(args.begin(), args.end())
{}

const vector<unique_ptr<Expression>> &ActionMapping::args() const
{
	return args_;
}
}
