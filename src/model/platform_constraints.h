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

#ifndef PLATFORM_CONSTRAINTS_H
#define PLATFORM_CONSTRAINTS_H

#include "platform_model.h"

namespace gologpp{

/**
 * Specifications of actions that trigger platform usage
 *
 * \a id corresponds to the action name
 * \a args holds the all arguments of the action, in case platform usage should
 * be triggered independent from some argument, it may be specified using the
 * generic value constructor
 */
struct actionSpec {
	::std::string id;
	::std::vector<shared_ptr<Value>> args;
};
typedef struct actionSpec ActionSpec;

/**
 * Different types of supported interconnection constraints.
 *
 * Constraints activate upon reaching a plan action \a pa.
 */
enum PlatformConstraintType {
	/** be in a set of target states within some time bounds */
	Future,
	/** upon reaching \a pa be in a set of target states */
	NoOp,
	/** have been in a set of targets satates within some time bounds */
	Past,
	/**
	 * be in one set of target states first, then within some time bounds reach
	 * another set of target states
	 */
	Until,
	/** Chained Until constraints (be in one set of target states, reach another,
	 * reach yet another, ...., until you reach a certain plan action
	 */
	UntilChain,
	/**
	 * have been in one set of target states first within some time bounds;
	 * eversince that, have been in another set of target states until reaching
	 * \a pa
	 */
	Since
};

/**
 * Target states together with associated time bounds.
 */
struct targetSpecs {
	::std::vector<::std::string> targets;
	Duration bounds;
};
typedef struct targetSpecs TargetSpecs;

/**
 * Platform constraint base structure.
 */
struct platformConstraint {
	PlatformConstraintType type;
	::std::vector<ActionSpec> activations;
	platformConstraint(PlatformConstraintType arg_type,
						const ::std::vector<ActionSpec> &arg_activations)
			: type(arg_type), activations(arg_activations){};
	virtual ~platformConstraint() = default;
};
typedef struct platformConstraint PlatformConstraint;

/**
 * Constraints that specify at most one set of target states.
 *
 * Suitable for #NoOp, #Future and #Past.
 */
struct unaryConstraint : PlatformConstraint {
	TargetSpecs specs;
	unaryConstraint(PlatformConstraintType arg_type,
						const ::std::vector<ActionSpec> &arg_activations,
						const TargetSpecs &arg_specs)
			: PlatformConstraint(arg_type, arg_activations), specs(arg_specs) {}
};
typedef struct unaryConstraint UnaryConstraint;

/**
 * Constraints that specify two set of target states and
 * one set of bounds.
 *
 * Suitable for #Future and #Since.
 */
struct binaryConstraint : PlatformConstraint {
	TargetSpecs specs;
	::std::vector<::std::string> pre_targets;
	binaryConstraint(PlatformConstraintType arg_type,
						 const ::std::vector<ActionSpec> &arg_activations,
						 const TargetSpecs &arg_specs,
						 const ::std::vector<::std::string> arg_pre_targets)
			: PlatformConstraint(arg_type, arg_activations), specs(arg_specs),
				pre_targets(arg_pre_targets) {}
};
typedef struct binaryConstraint BinaryConstraint;

/**
 * Chained constraints specifying a varying number of
 * target sates and bounds.
 *
 * Suitable for #UntilChain.
 */
struct chainConstraint : PlatformConstraint {
	::std::vector<TargetSpecs> specs_list;
	::std::vector<ActionSpec> activations_end;
	chainConstraint(PlatformConstraintType arg_type,
						const ::std::vector<ActionSpec> &arg_activations,
						const ::std::vector<TargetSpecs> &arg_specs_list,
						const ::std::vector<ActionSpec> &arg_activations_end)
			: PlatformConstraint(arg_type, arg_activations),
				specs_list(arg_specs_list), activations_end(arg_activations_end) {}
};
typedef struct chainConstraint ChainConstraint;

} // namespace gologpp
BOOST_FUSION_ADAPT_STRUCT(
	gologpp::TargetSpecs,
	(std::vector<::std::string>, targets)
	(gologpp::Duration, bounds)
)
BOOST_FUSION_ADAPT_STRUCT(
	gologpp::ActionSpec,
	(std::string, id)
	(std::vector<std::shared_ptr<gologpp::Value>>, args)
)


#endif /* !PLATFORM_CONSTRAINTS_H */
