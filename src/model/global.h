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

#ifndef GOLOGPP_GLOBAL_H_
#define GOLOGPP_GLOBAL_H_

#include "gologpp.h"
#include "utilities.h"
#include "language.h"


namespace gologpp {


class Global
: public Identifier
, public virtual AbstractLanguageElement
{
protected:
	Global(const string &name, const vector<shared_ptr<Variable>> &params);

	template<class GologT>
	Reference<GologT> *make_ref_(const vector<Expression *> &params = {}) {
		static_assert(std::is_base_of<Identifier, GologT>::value,
		"Cannot reference a type that is not derived from Identifier");
		return new Reference<GologT>(
			std::dynamic_pointer_cast<GologT>(this->shared_from_this()),
			std::move(params)
		);
	}

public:
	using ElementType = ModelElement;

	virtual ~Global() override = default;

	vector<shared_ptr<Variable>> &params();
	const vector<shared_ptr<Variable>> &params() const;
	shared_ptr<Variable> parameter(arity_t idx) const;
	void set_params(const vector<shared_ptr<Variable>> &params);

	Reference<Variable> *param_ref(const string &name);

	virtual void compile(AExecutionController &ctx) = 0;
	virtual ModelElement *ref(const vector<Expression *> &params = {}) = 0;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

private:
	vector<shared_ptr<Variable>> params_;
};



template<class T>
class Signified
: public Global
{
public:
	using SignifierT = T;

	using Global::Global;

	virtual SignifierT *ref(const vector<Expression *> &params = {}) = 0;
};


} // namespace gologpp


#endif
