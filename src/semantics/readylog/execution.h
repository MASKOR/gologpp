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

#ifndef READYLOG_EXECUTION_H_
#define READYLOG_EXECUTION_H_

#include <iostream>

#include <model/execution.h>

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

	virtual void compile(const Block &block) override;
	virtual void compile(const AbstractAction &action) override;
	virtual void compile(const Fluent &fluent) override;
	virtual void compile(const Function &function) override;
	virtual void compile(const Procedure &proc) override;

    virtual bool final(Block &program, History &history) override;
    virtual bool trans(Block &program, History &history) override;

	bool ec_query(EC_word t);
	void ec_cut();
	void ec_write(EC_word t);
	string to_string(EC_word t);

private:
    ReadylogContext(const eclipse_opts &options, unique_ptr<PlatformBackend> &&exec_backend);

	virtual void precompile_() override {}
	virtual void postcompile_() override;

    virtual void compile_term(const EC_word &term);
    std::string find_readylog();
    std::string find_boilerplate();

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
