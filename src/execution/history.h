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

#ifndef GOLOGPP_HISTORY_H_
#define GOLOGPP_HISTORY_H_

#include <model/language.h>
#include <model/scope.h>
#include <model/gologpp.h>
#include <model/semantics.h>

namespace gologpp {

class History : public LanguageElement<History>, public NoScopeOwner {
public:
	History() = default;
	History(History &&);

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

	DEFINE_ATTACH_SEMANTICS

	virtual string to_string(const string &pfx) const override;
};



template<>
class GeneralSemantics<History> : public virtual GeneralSemantics<ModelElement> {
public:
	GeneralSemantics<History>(History &history, AExecutionController &context);
	virtual ~GeneralSemantics<History>() override;

	virtual shared_ptr<Transition> get_last_transition() = 0;
	virtual void append(const Reference<AbstractAction> &) = 0;
	virtual void append(const Transition &) = 0;
	virtual void append(shared_ptr<Reference<AbstractAction>>);
	virtual bool should_progress() const = 0;
	virtual void progress() = 0;

	virtual void append_sensing_result(
		shared_ptr<Activity> a,
		const Expression &lhs,
		const Value &sensing_result
	) = 0;

	const History &element() const;
	virtual const ModelElement &model_element() const override;

	virtual AExecutionController &context() const override;

protected:
	History &history_;
	AExecutionController &context_;
};



} // namespace gologpp


#endif // GOLOGPP_HISTORY_H_
