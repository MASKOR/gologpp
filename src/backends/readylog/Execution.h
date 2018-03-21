#ifndef READYLOG_EXECUTION_H_
#define READYLOG_EXECUTION_H_

#include <gologpp/Execution.h>
#include "config.h"
#include <eclipseclass.h>

namespace gologpp {

template<>
class Implementation<History> : public HistoryImplementation {
public:
	Implementation(History &);
	virtual ~Implementation() override = default;

	virtual void append_exog(ExogTransition &&exog) override;
	virtual EC_ref current_history();
	virtual void set_current_history(EC_ref h);

private:
	EC_ref readylog_history_;
	size_t history_len_;
};


class EclipseContext : public ExecutionContext {
public:
    virtual ~EclipseContext() override;
    static EclipseContext &instance();

	virtual void compile(const Block &block) override;
	virtual void compile(const AbstractAction &action) override;
	virtual void compile(const AbstractFluent &fluent) override;
	virtual void compile(const AbstractFunction &function) override;

    virtual void compile(const EC_word &term);

    virtual bool final(Block &program, History &history) override;
    virtual bool trans(Block &program, History &history) override;

private:
    EclipseContext();

};


class EclipseError : public std::exception {
	using std::exception::exception;
};


} // namespace gologpp

#endif // READYLOG_EXECUTION_H_
