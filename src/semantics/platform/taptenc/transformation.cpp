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
#include <execution/context.h>

#include <taptenc/src/transformation.h>
#include <taptenc/src/constants.h>

#include <sstream>

namespace gologpp {


void TaptencTransformation::init(AExecutionContext &ctx)
{
	context_ = &ctx;

	std::unordered_map <
		shared_ptr<const platform::Component>,
		vector<std::reference_wrapper<platform::Constraint>>
	> platform_model;

	for (const auto &constraint : global_scope().constraints())
		platform_model[find_component_ref(constraint->rhs())]
		              .push_back(std::ref(*constraint));

	taptenc::transformation::Constraints::size_type i = 0;
	for (const auto &pair : platform_model) {
		tt_automata_.emplace_back(*pair.first->special_semantics().compile());
		for (const auto &constraint : pair.second)
			tt_constraints_[i].emplace_back(constraint.get().special_semantics().compile());
		++i;
	}
}


unique_ptr<Plan> TaptencTransformation::transform(Plan &&p)
{
	arg_storage_.clear();

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

AExecutionContext &TaptencTransformation::context()
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
	for (auto &ti : p.elements()) {
		if (ti.instruction().is_a<Transition>()) {
			Transition &trans = ti.instruction().cast<Transition>();
			std::string actstr = gologpp::to_string(trans.hook()) + "G" + trans->name();

			std::vector<std::string> argstr;
			for (auto &arg : trans.args())
				argstr.push_back(store_arg(dynamic_cast<const Value &>(*arg)));

			auto e = std::chrono::duration_cast<std::chrono::seconds>(ti.earliest_timepoint() - context().context_time());
			auto l = std::chrono::duration_cast<std::chrono::seconds>(
				Clock::duration(std::min(
					(ti.latest_timepoint() - context().context_time()).count(),
					static_cast<Clock::rep>(std::numeric_limits<std::chrono::seconds::rep>::max())
				))
			);

			rv.push_back(taptenc::PlanAction {
				taptenc::ActionName(actstr, argstr),
				taptenc::Bounds(
					boost::numeric_cast<taptenc::timepoint>(e.count()),
					boost::numeric_cast<taptenc::timepoint>(
						std::min(
							static_cast<decltype(l)::rep>(std::numeric_limits<taptenc::timepoint>::max()),
							l.count()
						)
					)
				),
				taptenc::Bounds(0, std::numeric_limits<taptenc::timepoint>::max())
			} );
		}
		else {
			/* TODO: This silently discards test markers and other plan elements. Need to insert identifying
			 * placeholders for other plan elements & retrieve them from storage when translating back */
			log(LogLevel::WRN) << "FIXME: Discarding plan action " << ti.instruction().str() << flush;
		}
	}
	return rv;
}


unique_ptr<Plan> TaptencTransformation::plan_taptenc_to_gpp(taptenc::timed_trace_t &&ttt)
{
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


std::string TaptencTransformation::store_arg(const Value &v)
{
	arg_storage_.push_back(v.copy());
	return "arg" + std::to_string(arg_storage_.size() - 1);
}


Value *TaptencTransformation::retrieve_arg(std::string taptenc_symbolic_arg)
{
	std::stringstream str(taptenc_symbolic_arg);
	str.seekg(3);
	size_t idx = std::numeric_limits<size_t>::max();
	str >> idx;
	if (idx == std::numeric_limits<size_t>::max())
		throw Bug("Cannot retrieve argument from storage: " + taptenc_symbolic_arg);

	return arg_storage_[idx];
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

	string act_name = tt_action.substr(hook_sep + 1, brace_opn);

	string argstr = tt_action.substr(brace_opn + 1, brace_cls);
	vector<unique_ptr<Expression>> args;
	while (argstr.size()) {
		auto arg_sep = argstr.find(taptenc::constants::VAR_SEP);
		args.emplace_back(retrieve_arg(argstr.substr(0, arg_sep)));
		if (arg_sep == std::string::npos)
			argstr = "";
		else
			argstr = argstr.substr(arg_sep + 1);
	}

	TimedInstruction ti {
		new Transition(
			global_scope().lookup_global<Action>(act_name),
			std::move(args),
			hook
		)
	};
	ti.set_earliest(context().context_time() + std::chrono::seconds(tt_time.earliest_start));
	ti.set_latest(context().context_time() + std::chrono::seconds(tt_time.earliest_start + tt_time.max_delay));

	return ti;
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
	string s_act = tt_action.substr(sp1_idx + 2, sp2_idx - 2);
	string s_to = tt_action.substr(sp2_idx);

	auto comp_idx_from = s_from.find(taptenc::constants::COMPONENT_SEP);
	if (comp_idx_from == std::string::npos)
		throw Bug("Component name missing in transition string: " + tt_action);
	string component_name_from = s_from.substr(0, comp_idx_from);
	string from_name = s_from.substr(comp_idx_from + 1);

	auto comp_idx_to = s_to.find(taptenc::constants::COMPONENT_SEP);
	if (comp_idx_to == std::string::npos)
		throw Bug("Component name missing in transition string: " + tt_action);
	string component_name_to = s_to.substr(0, comp_idx_to);
	string to_name = s_to.substr(comp_idx_to + 1);

	if (component_name_from != component_name_to)
		throw Bug("Inconsistent component names: " + tt_action);

	TimedInstruction rv { new Reference<platform::SwitchStateAction> {
		global_scope().lookup_global<platform::SwitchStateAction>("switch_state"),
		vector<Expression *> {
			new Value(get_type<StringType>(), component_name_from),
			new Value(get_type<StringType>(), from_name),
			new Value(get_type<StringType>(), to_name)
		}
	} };
	rv.set_earliest(context().context_time() + std::chrono::seconds(tt_time.earliest_start));
	rv.set_latest(context().context_time() + std::chrono::seconds(tt_time.earliest_start + tt_time.max_delay));

	return rv;
}



} // namespace gologpp
