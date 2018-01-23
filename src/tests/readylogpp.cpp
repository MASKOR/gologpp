#include <backends/readylog/Action.h>
#include <backends/readylog/Fluent.h>
#include <backends/readylog/Formula.h>
#include <backends/readylog/Reference.h>
#include <backends/readylog/Execution.h>

int main(int, const char **) {
	using namespace gologpp;

	readylog::ExecutionContext &ctx = readylog::ExecutionContext::instance();

	shared_ptr<readylog::Fluent> on = ctx.add_fluent(readylog::Fluent(generic::Fluent("on", {"X", "Y"})));
	shared_ptr<readylog::Action> put = ctx.add_action(readylog::Action(generic::Action("put", {"X", "Y"})));

	generic::Negation n(
			make_shared<Translatable<generic::FluentReference, EC_word>>(generic::FluentReference(on, put->args(), put->scope())));
}