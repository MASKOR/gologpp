/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "transformation.h"

#include <model/scope.h>
#include <model/platform/component.h>
#include <model/platform/constraint.h>
#include <model/platform/switch_state_action.h>
#include <model/logger.h>

#include <semantics/platform/taptenc/component.h>
#include <semantics/platform/taptenc/constraint.h>

#include <execution/plan.h>
#include <execution/controller.h>

#include <taptenc/src/transformation.h>
#include <taptenc/src/constants.h>

#include <sstream>

namespace gologpp {


void TaptencTransformation::init(AExecutionController &ctx)
{
	context_ = &ctx;

	// Must match constraints to the components the refer to.
	// Note: Each constraint must refer to exactly one component!
	for (const auto &constraint : global_scope().constraints())
		platform_model_[find_component_ref(constraint->rhs())]
			.push_back(std::ref(*constraint));

	for (const auto &pair : platform_model_) {
		tt_constraints_.emplace_back();
		for (const auto &constraint : pair.second)
			tt_constraints_.back().emplace_back(constraint.get().special_semantics().compile());
	}
}


unique_ptr<Plan> TaptencTransformation::transform(Plan &&p)
{
	if (p.elements().empty())
		return unique_ptr<Plan>(new Plan(std::move(p)));

	arg_to_sym_.clear();
	sym_to_arg_.clear();
	tt_update_automata();

	if (tt_automata_.size() == 0 && tt_constraints_.size() == 0)
		return unique_ptr<Plan>(new Plan(std::move(p)));
	else if (tt_automata_.size() == tt_constraints_.size())
		return plan_taptenc_to_gpp(
			taptenc::transformation::transform_plan(
				plan_gpp_to_taptenc(std::move(p)),
				tt_automata_,
				tt_constraints_
			)
		);
	else
		throw Unsupported("Taptenc semantics require at least one constraint for each component");
}


void TaptencTransformation::tt_update_automata()
{
	tt_automata_.clear();
	for (const auto &pair : platform_model_)
		tt_automata_.emplace_back(*pair.first->special_semantics().compile());
}


AExecutionController &TaptencTransformation::context()
{ return *context_; }


shared_ptr<const platform::Component> TaptencTransformation::find_component_ref(const platform::StateSpec &expr)
{
	using namespace platform;

	if (expr.is_a<StateAssertion>())
		return expr.cast<StateAssertion>().component().target();
	else if (expr.is_a<TemporalBinaryOperation<StateSpec>>()) {
		shared_ptr<const Component> lhs_ref = find_component_ref(
			expr.cast<TemporalBinaryOperation<StateSpec>>().lhs()
		);
		shared_ptr<const Component> rhs_ref = find_component_ref(
			expr.cast<TemporalBinaryOperation<StateSpec>>().rhs()
		);
		if (lhs_ref != rhs_ref)
			throw Unsupported("Taptenc semantics require that a state spec refer to one component only: " + expr.str());
		return lhs_ref;
	}
	else if (expr.is_a<BooleanConstraintOperation<StateSpec>>()) {
		shared_ptr<const Component> lhs_ref = find_component_ref(
			expr.cast<BooleanConstraintOperation<StateSpec>>().lhs()
		);
		shared_ptr<const Component> rhs_ref = find_component_ref(
			expr.cast<BooleanConstraintOperation<StateSpec>>().rhs()
		);
		if (lhs_ref != rhs_ref)
			throw Unsupported("Taptenc semantics require that a state spec refer to one component only: " + expr.str());
		return lhs_ref;
	}
	else if (expr.is_a<TemporalUnaryOperation<StateSpec>>())
		return find_component_ref(expr.cast<TemporalUnaryOperation<StateSpec>>().subject());
	else
		throw Unsupported("Unexpected constraint expression: " + expr.str());
}


vector<taptenc::PlanAction> TaptencTransformation::plan_gpp_to_taptenc(Plan &&p)
{
	vector<taptenc::PlanAction> rv;
	gologpp::Clock::time_point pbegin_t = p.elements().front().earliest_timepoint();
	log(LogLevel::DBG) << "=== Trafo input: {" << flush;
	for (auto &ti : p.elements()) {
		if (ti.instruction().is_a<Transition>()) {
			Transition &trans = ti.instruction().cast<Transition>();
			std::string actstr = gologpp::to_string(trans.hook()) + "G" + trans->name();

			std::vector<std::string> argstr;
			for (auto &arg : trans.args())
				argstr.push_back(store_arg(dynamic_cast<Value &>(*arg)));

			gologpp::Clock::rep earliest_shifted = std::max(0., (ti.earliest_timepoint() - pbegin_t).count());
			gologpp::Clock::rep latest_shifted = std::max(0., (ti.latest_timepoint() - pbegin_t).count());

			log(LogLevel::DBG) << "    " << trans << " [" << earliest_shifted << ", " << latest_shifted << "]" << flush;

			rv.push_back(taptenc::PlanAction {
				taptenc::ActionName(actstr, argstr),
				taptenc::Bounds(
					boost::numeric_cast<taptenc::timepoint>(earliest_shifted),
					boost::numeric_cast<taptenc::timepoint>(latest_shifted)
				),
				taptenc::Bounds(trans->duration().min.count(), trans->duration().max.count())
			} );
		}
		else {
			/* TODO: This silently discards test markers and other plan elements. Need to insert identifying
			 * placeholders for other plan elements & retrieve them from storage when translating back */
		}
	}
	return rv;
}


unique_ptr<Plan> TaptencTransformation::plan_taptenc_to_gpp(taptenc::timed_trace_t &&ttt)
{
	if (ttt.empty())
		throw Bug("Empty taptenc trace");
	unique_ptr<Plan> rv { new Plan() };
	for (auto &pair : ttt) {
		taptenc::groundedActionTime time = pair.first;
		for (const string &action_str : pair.second) {
			if (action_str.size()) {
				if (action_str.find("->") != std::string::npos)
					rv->append(parse_platform_action(action_str, time));
				else
					rv->append(parse_domain_action(action_str, time));
			}
		}
	}

	return rv;
}


std::string TaptencTransformation::store_arg(Value &v)
{
	auto it = arg_to_sym_.find(unique_ptr<Value>(v.copy()));
	if (it != arg_to_sym_.end())
		return it->second;
	else {
		size_t count = arg_to_sym_.size();
		std::string rv = "arg" + std::to_string(count);
		arg_to_sym_.insert({ unique_ptr<Value>(v.copy()), rv });
		sym_to_arg_.insert({ rv, unique_ptr<Value>(v.copy()) });
		return rv;
	}
}


Value *TaptencTransformation::retrieve_arg(std::string taptenc_symbolic_arg)
{
	auto it = sym_to_arg_.find(taptenc_symbolic_arg);
	if (it == sym_to_arg_.end())
		throw Bug("Cannot retrieve argument from storage: " + taptenc_symbolic_arg);
	return it->second->copy();
}


TimedInstruction TaptencTransformation::parse_domain_action(
	const std::string &tt_action,
	taptenc::groundedActionTime tt_time
) {
	auto hook_sep = tt_action.find('G');
	auto brace_opn = tt_action.find(taptenc::constants::OPEN_BRACKET);
	auto brace_cls = tt_action.find(taptenc::constants::CLOSED_BRACKET);

	string hookstr = tt_action.substr(0, hook_sep);
	Transition::Hook hook;
	if (hookstr == "start")
		hook = Transition::Hook::START;
	else if (hookstr == "end")
		hook = Transition::Hook::END;
	else if (hookstr == "fail")
		hook = Transition::Hook::FAIL;
	else if (hookstr == "cancel")
		hook = Transition::Hook::CANCEL;
	else if (hookstr == "finish")
		hook = Transition::Hook::FINISH;

	string act_name = tt_action.substr(hook_sep + 1, brace_opn - (hook_sep + 1));

	string argstr = tt_action.substr(brace_opn + 1, brace_cls - (brace_opn + 1));
	vector<unique_ptr<Expression>> args;
	while (argstr.size()) {
		auto arg_sep = argstr.find(taptenc::constants::VAR_SEP);
		args.emplace_back(retrieve_arg(argstr.substr(0, arg_sep)));
		if (arg_sep == std::string::npos)
			argstr = "";
		else
			argstr = argstr.substr(arg_sep + 1);
	}

	shared_ptr<Action> a = global_scope().lookup_global<Action>(act_name);
	if (!a)
		throw Bug("Unrecognized taptenc action name: " + act_name);

	TimedInstruction rv { new Transition(a, std::move(args), hook) };
	rv.set_earliest(gologpp::Clock::now() + std::chrono::seconds(tt_time.earliest_start));
	rv.set_latest(gologpp::Clock::now() + std::chrono::seconds(tt_time.earliest_start + tt_time.max_delay));

	if (rv.latest_timepoint() - rv.earliest_timepoint() < gologpp::Clock::duration(1))
		rv.set_latest(rv.latest_timepoint() + gologpp::Clock::duration(1));

	rv.instruction().attach_semantics(SemanticsFactory::get());

	return rv;
}


TimedInstruction TaptencTransformation::parse_platform_action(
	const std::string &tt_action,
	taptenc::groundedActionTime tt_time
) {
	auto sp1_idx = tt_action.find(' ');
	auto sp2_idx = tt_action.find(' ', sp1_idx + 1);

	if (sp1_idx == std::string::npos || sp2_idx == std::string::npos)
		throw Bug("Got unexpected platform transition: " + tt_action);

	string s_from = tt_action.substr(0, sp1_idx);
	string s_act = tt_action.substr(sp1_idx + 2, sp2_idx - 4 - sp1_idx);
	string s_to = tt_action.substr(sp2_idx + 1);

	auto comp_idx_from = s_from.find('G');
	if (comp_idx_from == std::string::npos)
		throw Bug("Component name missing in transition string: " + tt_action);
	string component_name_from = s_from.substr(0, comp_idx_from);
	string from_name = s_from.substr(comp_idx_from + 1);

	auto comp_idx_to = s_to.find('G');
	if (comp_idx_to == std::string::npos)
		throw Bug("Component name missing in transition string: " + tt_action);
	string component_name_to = s_to.substr(0, comp_idx_to);
	string to_name = s_to.substr(comp_idx_to + 1);

	if (component_name_from != component_name_to)
		throw Bug("Inconsistent component names: " + tt_action);

	shared_ptr<platform::SwitchStateAction> sa = global_scope().lookup_global<platform::SwitchStateAction>("switch_state");
	if (!sa)
		throw Bug("switch_state action is not defined");

	TimedInstruction rv { new Reference<platform::SwitchStateAction> {
		sa,
		vector<Expression *> {
			new Value(get_type<StringType>(), component_name_from),
			new Value(get_type<StringType>(), from_name),
			new Value(get_type<StringType>(), to_name)
		}
	} };
	rv.set_earliest(gologpp::Clock::now() + std::chrono::seconds(tt_time.earliest_start));
	rv.set_latest(gologpp::Clock::now() + std::chrono::seconds(tt_time.earliest_start + tt_time.max_delay));

	if (rv.latest_timepoint() - rv.earliest_timepoint() < gologpp::Clock::duration(1))
		rv.set_latest(rv.latest_timepoint() + gologpp::Clock::duration(1));

	rv.instruction().attach_semantics(SemanticsFactory::get());

	return rv;
}



} // namespace gologpp
