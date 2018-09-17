#ifndef READYLOG_EXECUTION_H_
#define READYLOG_EXECUTION_H_

#include <iostream>

#include <model/execution.h>
#include "config.h"
#include <eclipseclass.h>
#include "implementation.h"

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>

namespace gologpp {

template<>
class Implementation<History> : public HistoryImplementation {
public:
	Implementation(History &);
	virtual ~Implementation() override = default;

	virtual void append_exog(ExogTransition &&exog) override;
	EC_word current_history();
	void set_current_history(EC_word h);

private:
	EC_word readylog_history_;
};


EC_word operator && (const EC_word &lhs, const EC_word &rhs);


struct eclipse_opts {
	bool trace;
	bool guitrace;
};


class EclipseContext : public ExecutionContext {
public:
    virtual ~EclipseContext() override;
    static void init(unique_ptr<AExecutionBackend> &&, const eclipse_opts &options);
    static void shutdown();
    static EclipseContext &instance();

	virtual void compile(const Block &block) override;
	virtual void compile(const AbstractAction &action) override;
	virtual void compile(const AbstractFluent &fluent) override;
	virtual void compile(const AbstractFunction &function) override;

    virtual void compile_term(const EC_word &term);

    virtual bool final(Block &program, History &history) override;
    virtual bool trans(Block &program, History &history) override;

	bool ec_query(EC_word t)
	{
		/* Insanity ensues:
		   Since the EC_ref::cut_to() mechanism seems to be broken, as well as post_goal("!"),
		   which simply has no effect, once/1 seems to be the ONLY way to cut choicepoints from
		   posted goals.

		   More info:
		   http://eclipseclp.org/doc/embedding/embroot081.html
		   https://sourceforge.net/p/eclipse-clp/mailman/message/23203000/
		   http://eclipseclp.org/archive/eclipse-users/0704.html
		*/
		post_goal(::term(EC_functor("writeln", 1), t));
		post_goal(::term(EC_functor("once", 1), t));
		post_goal(::term(EC_functor("writeln", 1), t));
		last_rv_ = EC_resume(*ec_start_);
		std::cout << std::endl;

		return last_rv_ == EC_status::EC_succeed;
	}

	void ec_cut();

	void ec_write(EC_word t);


private:
	EC_ref *ec_start_;
	int last_rv_;
	eclipse_opts options_;

	static unique_ptr<EclipseContext> instance_;

    EclipseContext(unique_ptr<AExecutionBackend> &&exec_backend, const eclipse_opts &options);
};


class EclipseError : public std::exception {
	using std::exception::exception;
};


} // namespace gologpp

#endif // READYLOG_EXECUTION_H_
