#ifndef GOLOGPP_MAPPING_H_
#define GOLOGPP_MAPPING_H_


#include "utilities.h"
#include "expressions.h"
#include "scope.h"

#include <unordered_map>
#include <boost/fusion/include/vector.hpp>

namespace gologpp {


template<class T1, class T2>
#ifdef BOOST_FUSION_HAS_VARIADIC_VECTOR
using fusion_wtf_vector = boost::fusion::vector<T1, T2>;
#else
using fusion_wtf_vector = boost::fusion::vector2<T1, T2>;
#endif


class ActionMapping : public LanguageElement<ActionMapping>, public NoScopeOwner {
public:
	using ArgMapping = std::unordered_map<
		string,
		unique_ptr<Expression>
	>;

	ActionMapping(
		const string &backend_name,
		vector<fusion_wtf_vector<string, Expression *>> arg_mapping
	);

	const string &backend_name() const;
	const Expression &mapped_expr(const string &arg_name) const;
	bool is_mapped(const string &arg_name) const;

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

	const ArgMapping &arg_mapping() const;

	void set_action(AbstractAction *);

private:
	AbstractAction *action_;
	string backend_name_;
	ArgMapping arg_mapping_;
};


}


#endif // GOLOGPP_MAPPING_H_
