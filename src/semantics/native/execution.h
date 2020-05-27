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

#include <execution/context.h>

#include "semantics.h"
#include "history.h"


namespace gologpp {


class NativeContext : public AExecutionContext {
public:
	virtual ~NativeContext() override;
	static void init(unique_ptr<PlatformBackend> &&backend = nullptr);
	static void shutdown();
	static NativeContext &instance();

	virtual void precompile() override {}
	virtual void compile(const Block &block) override;
	virtual void compile(const AbstractAction &action) override;
	virtual void compile(const Fluent &fluent) override;
	virtual void compile(const Function &function) override;
	virtual void compile(const Procedure &proc) override;
	virtual void postcompile() override;

	shared_ptr<Transition> wait_for_end(Activity &a);

	virtual void run(Block &&program) override;

private:
    NativeContext(unique_ptr<PlatformBackend> &&exec_backend);

	static unique_ptr<NativeContext> instance_;
};


class NativeFailure {
};


} // namespace gologpp

#endif // READYLOG_EXECUTION_H_
