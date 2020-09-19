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

#pragma once

#include <model/platform/constraint.h>

#include <semantics/platform/taptenc/semantics.h>

#include <taptenc/src/encoder/enc_interconnection_info.h>


namespace gologpp {


template<>
class Semantics<platform::Constraint>
: public GeneralSemantics<platform::Constraint>
{
public:
	Semantics(const platform::Constraint &elem, AExecutionController &context);

	std::unique_ptr<taptenc::EncICInfo> compile();

private:
	bool is_chain(const platform::ActionSpec &);

	static size_t count_;
	string id;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::ActionSpec>
{
public:
	virtual std::vector<taptenc::ActionName> compile() = 0;

	virtual std::pair <
		std::vector<taptenc::ActionName>,
		std::vector<taptenc::ActionName>
	> compile_chain() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::ActionHook>
: public GeneralSemantics<platform::ActionHook>
, public Semantics<platform::ActionSpec>
{
public:
	using GeneralSemantics<platform::ActionHook>::GeneralSemantics;

	virtual std::vector<taptenc::ActionName> compile() override;

	virtual std::pair <
		std::vector<taptenc::ActionName>,
		std::vector<taptenc::ActionName>
	> compile_chain() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::BooleanConstraintOperation<platform::ActionSpec>>
: public GeneralSemantics<platform::BooleanConstraintOperation<platform::ActionSpec>>
, public Semantics<platform::ActionSpec>
{
public:
	using GeneralSemantics<platform::BooleanConstraintOperation<platform::ActionSpec>>::GeneralSemantics;

	virtual std::vector<taptenc::ActionName> compile() override;

	virtual std::pair <
		std::vector<taptenc::ActionName>,
		std::vector<taptenc::ActionName>
	> compile_chain() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::TemporalUnaryOperation<platform::ActionSpec>>
: public GeneralSemantics<platform::TemporalUnaryOperation<platform::ActionSpec>>
{
public:
	Semantics(const platform::TemporalUnaryOperation<platform::ActionSpec> &elem, AExecutionController &context);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::TemporalBinaryOperation<platform::ActionSpec>>
: public GeneralSemantics<platform::TemporalBinaryOperation<platform::ActionSpec>>
{
public:
	Semantics(const platform::TemporalBinaryOperation<platform::ActionSpec> &elem, AExecutionController &context);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::During>
: public GeneralSemantics<platform::During>
, public Semantics<platform::ActionSpec>
{
public:
	using GeneralSemantics<platform::During>::GeneralSemantics;

	virtual std::vector<taptenc::ActionName> compile() override;

	virtual std::pair <
		std::vector<taptenc::ActionName>,
		std::vector<taptenc::ActionName>
	> compile_chain() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::StateSpec>
{
public:
	virtual std::unique_ptr<taptenc::EncICInfo> compile(
		string id,
		const std::vector<taptenc::ActionName> activations
	) = 0;

	virtual std::unique_ptr<taptenc::EncICInfo> compile_chain(
		string id,
		const std::vector<taptenc::ActionName> activations,
		const std::vector<taptenc::ActionName> activations_end
	) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::StateAssertion>
: public GeneralSemantics<platform::StateAssertion>
, public Semantics<platform::StateSpec>
{
public:
	using GeneralSemantics<platform::StateAssertion>::GeneralSemantics;

	virtual std::unique_ptr<taptenc::EncICInfo> compile(
		string id,
		const std::vector<taptenc::ActionName> activations
	) override;

	virtual std::unique_ptr<taptenc::EncICInfo> compile_chain(
		string id,
		const std::vector<taptenc::ActionName> activations,
		const std::vector<taptenc::ActionName> activations_end
	) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::BooleanConstraintOperation<platform::StateSpec>>
: public GeneralSemantics<platform::BooleanConstraintOperation<platform::StateSpec>>
, public Semantics<platform::StateSpec>
{
public:
	using GeneralSemantics<platform::BooleanConstraintOperation<platform::StateSpec>>::GeneralSemantics;

	virtual std::unique_ptr<taptenc::EncICInfo> compile(
		string id,
		const std::vector<taptenc::ActionName> activations
	) override;

	virtual std::unique_ptr<taptenc::EncICInfo> compile_chain(
		string id,
		const std::vector<taptenc::ActionName> activations,
		const std::vector<taptenc::ActionName> activations_end
	) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::TemporalUnaryOperation<platform::StateSpec>>
: public GeneralSemantics<platform::TemporalUnaryOperation<platform::StateSpec>>
, public Semantics<platform::StateSpec>
{
public:
	using GeneralSemantics<platform::TemporalUnaryOperation<platform::StateSpec>>::GeneralSemantics;

	virtual std::unique_ptr<taptenc::EncICInfo> compile(
		string id,
		const std::vector<taptenc::ActionName> activations
	) override;

	virtual std::unique_ptr<taptenc::EncICInfo> compile_chain(
		string id,
		const std::vector<taptenc::ActionName> activations,
		const std::vector<taptenc::ActionName> activations_end
	) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::TemporalBinaryOperation<platform::StateSpec>>
: public GeneralSemantics<platform::TemporalBinaryOperation<platform::StateSpec>>
, public Semantics<platform::StateSpec>
{
public:
	using GeneralSemantics<platform::TemporalBinaryOperation<platform::StateSpec>>::GeneralSemantics;

	virtual std::unique_ptr<taptenc::EncICInfo> compile(
		string id,
		const std::vector<taptenc::ActionName> activations
	) override;

	virtual std::unique_ptr<taptenc::EncICInfo> compile_chain(
		string id,
		const std::vector<taptenc::ActionName> activations,
		const std::vector<taptenc::ActionName> activations_end
	) override;
};


} // namespace gologpp
