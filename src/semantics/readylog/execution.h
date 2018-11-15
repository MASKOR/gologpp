#ifndef READYLOG_EXECUTION_H_
#define READYLOG_EXECUTION_H_

#include <iostream>

#include <model/execution.h>

#include "config.h"
#include "semantics.h"
#include "history.h"


namespace gologpp {


struct eclipse_opts {
	bool trace;
	bool guitrace;
};

class ReadylogContext : public ExecutionContext {
public:
	virtual ~ReadylogContext() override;
	static void init(unique_ptr<PlatformBackend> &&, const eclipse_opts &options);
	static void shutdown();
	static ReadylogContext &instance();

	virtual void compile(const Block &block) override;
	virtual void compile(const AbstractAction &action) override;
	virtual void compile(const AbstractFluent &fluent) override;
	virtual void compile(const AbstractFunction &function) override;

    virtual bool final(Block &program, History &history) override;
    virtual bool trans(Block &program, History &history) override;

	bool ec_query(EC_word t);
	void ec_cut();
	void ec_write(EC_word t);

private:
    ReadylogContext(unique_ptr<PlatformBackend> &&exec_backend, const eclipse_opts &options);

    virtual void compile_term(const EC_word &term);

	EC_ref *ec_start_;
	int last_rv_;
	eclipse_opts options_;
	static unique_ptr<ReadylogContext> instance_;
};


class EclipseError : public std::exception {
	using std::exception::exception;
};


} // namespace gologpp

#endif // READYLOG_EXECUTION_H_
