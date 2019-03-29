#ifndef READYLOG_EXECUTION_H_
#define READYLOG_EXECUTION_H_

#include <iostream>

#include <model/execution.h>

#include "config.h"
#include "semantics.h"
#include "history.h"


namespace gologpp {


struct eclipse_opts {
	bool trace = false;
	bool guitrace = false;
	bool toplevel = false;
};

class ReadylogContext : public ExecutionContext {
public:
	virtual ~ReadylogContext() override;
	static void init(const eclipse_opts &options = {false, false}, unique_ptr<PlatformBackend> &&backend = nullptr);
	static void shutdown();
	static ReadylogContext &instance();

	virtual void precompile() override {}
	virtual void compile(const Block &block) override;
	virtual void compile(const AbstractAction &action) override;
	virtual void compile(const AbstractFluent &fluent) override;
	virtual void compile(const AbstractFunction &function) override;
	virtual void postcompile() override;

    virtual bool final(Block &program, History &history) override;
    virtual bool trans(Block &program, History &history) override;

	bool ec_query(EC_word t);
	void ec_cut();
	void ec_write(EC_word t);
	string to_string(EC_word t);

private:
    ReadylogContext(const eclipse_opts &options, unique_ptr<PlatformBackend> &&exec_backend);

    virtual void compile_term(const EC_word &term);

	EC_ref *ec_start_;
	int last_rv_;
	eclipse_opts options_;
	static unique_ptr<ReadylogContext> instance_;
};


class EclipseError : public std::runtime_error {
	using std::runtime_error::runtime_error;
};


} // namespace gologpp

#endif // READYLOG_EXECUTION_H_
