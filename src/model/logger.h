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

#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <sstream>
#include <thread>
#include <stack>
#include <execution/clock.h>

namespace gologpp {


enum class LogLevel {
	ERR = 0,
	WRN,
	NFY,
	INF,
	DBG,
};


LogLevel &operator--(LogLevel &l);
LogLevel &operator++(LogLevel &l);


class Logger {
private:
	Logger();
public:
	virtual ~Logger();
	Logger &level(const LogLevel &lvl);
	Logger &flush();
	static Logger &instance();
	LogLevel &log_lvl();

	Logger &operator << (const std::string &msg);
	Logger &operator << (const unsigned int i);
	Logger &operator << (const int i);
	Logger &operator << (const float &d);
	Logger &operator << (const double &d);
	Logger &operator << (Logger & (*pf_flush)(Logger &));
	Logger &operator << (const char *msg);
	Logger &operator << (char *msg);
	Logger &operator << (const Clock::time_point &tm);
	Logger &operator << (const std::thread::id &id);


	template<class GologT>
	typename std::enable_if<
		std::is_base_of<ModelElement, GologT>::value,
		Logger &
	>::type
	operator << (const GologT &obj)
	{ return *this << obj.to_string(""); }


	template<class GologT>
	typename std::enable_if<
		std::is_base_of<ModelElement, GologT>::value,
		Logger &
	>::type
	operator << (const GologT *obj)
	{ return *this << (obj ? obj->to_string("") : "nullptr"); }


	template<class GologT>
	typename std::enable_if<
		std::is_base_of<ModelElement, GologT>::value,
		Logger &
	>::type
	operator << (const shared_ptr<GologT> &obj)
	{ return *this << (obj ? obj->to_string("") : "nullptr"); }


	template<class ElemT>
	Logger &
	operator << (const vector<ElemT> &v)
	{
		*this << "[";
		typename vector<ElemT>::const_iterator it = v.begin();
		for (const ElemT &e : v) {
			*this << e;
			if (++it < v.cend())
				*this << ", ";
		}
		return *this << "]";
	}


	template<class ElemT>
	Logger &
	operator << (const std::list<ElemT> &v)
	{
		*this << "[";
		typename std::list<ElemT>::const_iterator it = v.begin();
		for (const ElemT &e : v) {
			*this << e;
			if (++it != v.cend())
				*this << ", ";
		}
		return *this << "]";
	}


	template<class T>
	Logger &
	operator << (const std::reference_wrapper<T> &v)
	{ return *this << v.get(); }


	virtual void output_message(const std::string &msg);

	static string color_escape(LogLevel lvl);

	class Guard {
	public:
		Guard(LogLevel loglvl);
		~Guard();
	};

private:
	std::string &msg_pfx();

	static thread_local std::unique_ptr<Logger> instance_;

	bool syslog_;
	bool have_tty_;
	static LogLevel global_loglvl_;
	LogLevel msg_lvl_;
	std::string msg_pfx_;
	std::exception_ptr exception_;
	static std::mutex mutex_;
	std::stack<LogLevel> local_msg_lvl_;

	static constexpr const char *c_black = "\033[0;30m";
	static constexpr const char *c_darkgray = "\033[1;30m";
	static constexpr const char *c_red = "\033[0;31m";
	static constexpr const char *c_lightred = "\033[1;31m";
	static constexpr const char *c_green = "\033[0;32m";
	static constexpr const char *c_lightgreen = "\033[1;32m";
	static constexpr const char *c_brown = "\033[0;33m";
	static constexpr const char *c_yellow = "\033[1;33m";
	static constexpr const char *c_blue = "\033[0;34m";
	static constexpr const char *c_lightblue = "\033[1;34m";
	static constexpr const char *c_purple = "\033[0;35m";
	static constexpr const char *c_lightpurple = "\033[1;35m";
	static constexpr const char *c_cyan = "\033[0;36m";
	static constexpr const char *c_lightcyan = "\033[1;36m";
	static constexpr const char *c_lightgray = "\033[2;37m";
	static constexpr const char *c_white = "\033[1;37m";
	static constexpr const char *c_normal = "\033[0;39m";
};

Logger &flush(Logger &l);
Logger &log(LogLevel lvl);



} // namespace gologpp
