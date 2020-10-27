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

#ifndef READYLOG_HISTORY_H_
#define READYLOG_HISTORY_H_

#include "semantics.h"
#include "utilities.h"

#include <execution/history.h>


namespace gologpp {


template<>
class Semantics<History>
: public GeneralSemantics<History>
, public ReadylogSemantics
{
public:
	Semantics(History &, ReadylogContext &context);
	virtual ~Semantics() override = default;

	virtual shared_ptr<Transition> get_last_transition() override;
	virtual void append(const Transition &exog) override;
	virtual void append(const Reference<AbstractAction> &exog) override;
	virtual bool should_progress() const override;
	virtual void progress() override;

	virtual void append_sensing_result(
		shared_ptr<Activity> a,
		const Expression &lhs,
		const Value &sensing_result
	) override;

	void extend_history(EC_word h);
	bool has_changed() const;

	virtual EC_word plterm() override;
	EC_word plterm() const;

private:
	EC_word get_history_head();

	ManagedTerm readylog_history_;
	bool has_changed_;
};



} // namespace gologpp

#endif // READYLOG_HISTORY_H_
