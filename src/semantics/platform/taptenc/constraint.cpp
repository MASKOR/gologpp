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

#include "constraint.h"
#include <semantics/platform/taptenc/reference.h>

#include <model/platform/constraint.h>
#include <model/platform/reference.h>

#include <taptenc/src/encoder/enc_interconnection_info.h>
#include <taptenc/src/constants.h>

namespace gologpp {


std::string encode_alphabetic(size_t n)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

size_t Semantics<platform::Constraint>::count_ = 0;


Semantics<platform::Constraint>::Semantics(const platform::Constraint &elem, AExecutionContext &context)
: GeneralSemantics<platform::Constraint>(elem, context)
, id(encode_alphabetic(count_++))
{}


std::unique_ptr<taptenc::EncICInfo> Semantics<platform::Constraint>::compile()
{
	if (is_chain(element().lhs())) {
		std::vector<taptenc::ActionName> activations, activations_end;
		std::tie(activations, activations_end) = element().lhs().semantics<platform::ActionSpec>().compile_chain();
		return element().rhs().semantics<platform::StateSpec>().compile_chain(id, activations, activations_end);
	}
	else
		return element().rhs().semantics<platform::StateSpec>().compile(
			id,
			{ element().lhs().semantics<platform::ActionSpec>().compile() }
		);
}


bool Semantics<platform::Constraint>::is_chain(const platform::ActionSpec &as)
{
	using namespace platform;
	using During = platform::During;

	try {
		auto &bo = as.cast<BooleanConstraintOperation<ActionSpec>>();
		return bo.op() == BooleanConstraintOperation<ActionSpec>::Operator::OR
			&& is_chain(bo.lhs())
			&& is_chain(bo.rhs())
		;
	} catch (std::bad_cast &) {
		return as.is_a<During>();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<taptenc::ActionName> Semantics<platform::ActionHook>::compile()
{
	string hookname;
	switch (element().hook()) {
	case platform::ActionHook::Hook::START:
		hookname = taptenc::constants::START_PA;
		break;
	case platform::ActionHook::Hook::END:
		hookname = taptenc::constants::END_PA;
		break;
	default:
		throw Unsupported("Unsupported action hook: " + element().str());
	}

	std::vector<std::string> args;
	for (auto &a : element().action().args())
		args.push_back(a->cast<Value>().string_representation());

	return { taptenc::ActionName(hookname + element().action()->name(), args) };
}


std::pair<std::vector<taptenc::ActionName>, std::vector<taptenc::ActionName> > Semantics<platform::ActionHook>::compile_chain()
{ throw Unsupported("Unsupported action spec"); }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<taptenc::ActionName> Semantics<platform::BooleanConstraintOperation<platform::ActionSpec> >::compile()
{
	if (element().op() != platform::BooleanConstraintOperation<platform::ActionSpec>::Operator::OR)
		throw Unsupported("Only disjunction supported by taptenc semantics: " + element().str());

	auto rv = element().lhs().semantics<platform::ActionSpec>().compile();
	auto rhs = element().rhs().semantics<platform::ActionSpec>().compile();
	rv.insert(rv.end(), rhs.begin(), rhs.end());
	return rv;
}


std::pair<std::vector<taptenc::ActionName>, std::vector<taptenc::ActionName>> Semantics<platform::BooleanConstraintOperation<platform::ActionSpec> >::compile_chain()
{
	if (element().op() != platform::BooleanConstraintOperation<platform::ActionSpec>::Operator::OR)
		throw Unsupported("Only disjunction supported by taptenc semantics: " + element().str());

	std::vector<taptenc::ActionName> rv, rv_end, rhs, rhs_end;

	std::tie(rv, rv_end) = element().lhs().semantics<platform::ActionSpec>().compile_chain();
	std::tie(rhs, rhs_end) = element().rhs().semantics<platform::ActionSpec>().compile_chain();

	rv.insert(rv.end(), rhs.begin(), rhs.end());
	rv_end.insert(rv_end.end(), rhs_end.begin(), rhs_end.end());

	return { rv, rv_end };
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<taptenc::ActionName> Semantics<platform::During>::compile()
{ throw Unsupported("Unsupported action spec"); }


std::pair<std::vector<taptenc::ActionName>, std::vector<taptenc::ActionName>>
Semantics<platform::During>::compile_chain()
{
	std::vector<std::string> args;
	for (auto &a : element().action().args())
		args.push_back(a->cast<Value>().string_representation());

	return {
		{ taptenc::ActionName(taptenc::constants::START_PA + element().action()->name(), args) },
		{ taptenc::ActionName(taptenc::constants::END_PA + element().action()->name(), args) }
	};
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<taptenc::EncICInfo> Semantics<platform::StateSpec>::compile(
	string /*id*/,
	const std::vector<taptenc::ActionName> /*activations*/
)
{ throw Unsupported("Unsupported state spec"); }


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<taptenc::EncICInfo> Semantics<platform::StateAssertion>::compile(
	string id,
	const std::vector<taptenc::ActionName> activations
) {
	return std::unique_ptr<taptenc::EncICInfo>(new taptenc::UnaryInfo(
		id,
		taptenc::ICType::NoOp,
		activations,
		taptenc::TargetSpecs(
			taptenc::Bounds(),
			{ element().state().semantics().compile() }
		)
	));
}


std::unique_ptr<taptenc::EncICInfo> Semantics<platform::StateAssertion>::compile_chain(
	string id,
	const std::vector<taptenc::ActionName> activations,
	const std::vector<taptenc::ActionName> activations_end
) {
	return std::unique_ptr<taptenc::EncICInfo>(new taptenc::ChainInfo(
		id,
		taptenc::ICType::NoOp,
		activations,
		{ taptenc::TargetSpecs(
			taptenc::Bounds(),
			{ element().state().semantics().compile() }
		) },
		activations_end
	));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<taptenc::EncICInfo> Semantics<platform::BooleanConstraintOperation<platform::StateSpec> >::compile(
	string id,
	const std::vector<taptenc::ActionName> activations
) {
	if (element().op() != platform::BooleanConstraintOperation<platform::StateSpec>::Operator::OR)
		throw Unsupported("Only disjunction supported by taptenc semantics: " + element().str());

	auto tt_lhs = element().lhs().semantics<platform::StateSpec>().compile(id, activations);
	auto tt_rhs = element().rhs().semantics<platform::StateSpec>().compile(id, activations);

	if (tt_lhs->type != tt_rhs->type)
		throw Unsupported("Mixed temporal operators not supported by taptenc semantics: " + element().str());

	{
		taptenc::BinaryInfo *tt_bi_lhs = dynamic_cast<taptenc::BinaryInfo *>(tt_lhs.get());
		if (tt_bi_lhs) {
			taptenc::BinaryInfo *tt_bi_rhs = dynamic_cast<taptenc::BinaryInfo *>(tt_rhs.get());
			if (tt_bi_lhs->specs.bounds != tt_bi_rhs->specs.bounds)
				throw Unsupported("All temporal operators must have the same bounds: " + element().str());

			taptenc::TargetSpecs specs = tt_bi_lhs->specs;
			specs.targets.insert(specs.targets.end(), tt_bi_rhs->specs.targets.begin(), tt_bi_rhs->specs.targets.end());

			std::vector<taptenc::State> pre_targets = tt_bi_lhs->pre_targets;
			pre_targets.insert(pre_targets.end(), tt_bi_rhs->pre_targets.begin(), tt_bi_rhs->pre_targets.end());

			return std::unique_ptr<taptenc::EncICInfo>(new taptenc::BinaryInfo(
				id,
				tt_lhs->type,
				activations,
				specs,
				pre_targets
			));
		}
	}

	{
		taptenc::UnaryInfo *tt_ui_lhs = dynamic_cast<taptenc::UnaryInfo *>(tt_lhs.get());
		if (tt_ui_lhs) {
			taptenc::UnaryInfo *tt_ui_rhs = dynamic_cast<taptenc::UnaryInfo *>(tt_rhs.get());
			if (tt_ui_lhs->specs.bounds != tt_ui_rhs->specs.bounds)
				throw Unsupported("All temporal operators must have the same bounds: " + element().str());

			taptenc::TargetSpecs specs = tt_ui_lhs->specs;
			specs.targets.insert(specs.targets.end(), tt_ui_rhs->specs.targets.begin(), tt_ui_rhs->specs.targets.end());

			return std::unique_ptr<taptenc::EncICInfo>(new taptenc::UnaryInfo(
				id,
				tt_lhs->type,
				activations,
				specs
			));
		}
	}

	throw Bug("Got unexpected taptenc::EncICInfo from: " + element().str());
}


std::unique_ptr<taptenc::EncICInfo> Semantics<platform::BooleanConstraintOperation<platform::StateSpec> >::compile_chain(
	string id,
	const std::vector<taptenc::ActionName> activations,
	const std::vector<taptenc::ActionName> activations_end
) {
	if (element().op() != platform::BooleanConstraintOperation<platform::StateSpec>::Operator::OR)
		throw Unsupported("Only disjunction supported by taptenc semantics: " + element().str());

	auto tt_lhs = element().lhs().semantics<platform::StateSpec>().compile(id, activations);
	auto tt_rhs = element().rhs().semantics<platform::StateSpec>().compile(id, activations);

	// Can't have disjunctions over temporal expressions in an UntilChain
	if (tt_lhs->type != taptenc::ICType::NoOp || tt_rhs->type != taptenc::ICType::NoOp)
		throw Unsupported("Unsupported state spec: " + element().str());

	auto u_lhs = dynamic_cast<taptenc::UnaryInfo &>(*tt_lhs);
	auto u_rhs = dynamic_cast<taptenc::UnaryInfo &>(*tt_rhs);

	taptenc::TargetSpecs state_disjunction = u_lhs.specs;
	state_disjunction.targets.insert(
		state_disjunction.targets.end(),
		u_rhs.specs.targets.begin(),
		u_rhs.specs.targets.end()
	);

	return std::unique_ptr<taptenc::EncICInfo>(new taptenc::ChainInfo(
		id,
		taptenc::ICType::UntilChain,
		activations,
		{ state_disjunction },
		activations_end
	));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<taptenc::EncICInfo> Semantics<platform::TemporalUnaryOperation<platform::StateSpec>>::compile(
	string id,
	const std::vector<taptenc::ActionName> activations
) {
	auto tt_subj = element().subject().semantics<platform::StateSpec>().compile(id, activations);
	if (tt_subj->type != taptenc::ICType::NoOp)
		throw Unsupported("Unsupported state spec: " + element().str());

	taptenc::ICType tt_type;
	switch (element().op()) {
	case platform::TemporalUnaryOperation<platform::StateSpec>::Operator::FUTURE:
		tt_type = taptenc::ICType::Future;
		break;
	case platform::TemporalUnaryOperation<platform::StateSpec>::Operator::PAST:
		tt_type = taptenc::ICType::Past;
		break;
	default:
		throw Unsupported("Unsupported temporal operator: " + element().str());
	}

	taptenc::UnaryInfo *unary_subj = dynamic_cast<taptenc::UnaryInfo *>(tt_subj.get());
	unary_subj->specs.bounds = taptenc::Bounds(
		element().lower_bound().count(),
		element().upper_bound().count()
	);
	return std::unique_ptr<taptenc::EncICInfo>(new taptenc::UnaryInfo(
		id,
		tt_type,
		activations,
		unary_subj->specs
	) );
}

std::unique_ptr<taptenc::EncICInfo> Semantics<platform::TemporalUnaryOperation<platform::StateSpec>>::compile_chain(
	string /*id*/,
	const std::vector<taptenc::ActionName> /*activations*/,
	const std::vector<taptenc::ActionName> /*activations_end*/
)
{ throw Unsupported("Unsupported state spec"); }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<taptenc::EncICInfo> Semantics<platform::TemporalBinaryOperation<platform::StateSpec> >::compile(
	string id,
	const std::vector<taptenc::ActionName> activations
) {
	auto tt_lhs = element().lhs().semantics<platform::StateSpec>().compile(id, activations);
	auto tt_rhs = element().rhs().semantics<platform::StateSpec>().compile(id, activations);

	if (tt_lhs->type != taptenc::ICType::NoOp || tt_rhs->type != taptenc::ICType::NoOp)
		throw Unsupported("Unsupported state spec: " + element().str());

	taptenc::ICType tt_type;
	switch(element().op()) {
	case platform::TemporalBinaryOperation<platform::StateSpec>::Operator::SINCE:
		tt_type = taptenc::ICType::Since;
		break;
	case platform::TemporalBinaryOperation<platform::StateSpec>::Operator::UNTIL:
		tt_type = taptenc::ICType::Until;
		break;
	}

	taptenc::UnaryInfo &tt_ui_lhs = dynamic_cast<taptenc::UnaryInfo &>(*tt_lhs);
	taptenc::UnaryInfo &tt_ui_rhs = dynamic_cast<taptenc::UnaryInfo &>(*tt_rhs);

	return std::unique_ptr<taptenc::EncICInfo>(new taptenc::BinaryInfo(
		id,
		tt_type,
		activations,
		tt_ui_rhs.specs,
		tt_ui_lhs.specs.targets
	) );
}


std::unique_ptr<taptenc::EncICInfo> Semantics<platform::TemporalBinaryOperation<platform::StateSpec>>::compile_chain(
	string /*id*/,
	const std::vector<taptenc::ActionName> /*activations*/,
	const std::vector<taptenc::ActionName> /*activations_end*/
)
{ throw Unsupported("Unsupported state spec"); }




} // namespace gologpp
