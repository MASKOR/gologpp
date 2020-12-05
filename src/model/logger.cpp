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

#include "logger.h"
#include <model/error.h>

#include <iostream>
#include <iomanip>

#include <unistd.h>

namespace gologpp {


thread_local std::unique_ptr<Logger> Logger::instance_;
std::mutex Logger::mutex_;
LogLevel Logger::log_lvl_(LogLevel::INF);


LogLevel &operator--(LogLevel &l)
{
	if (l == LogLevel::DBG)
		l = LogLevel::INF;
	else if (l == LogLevel::INF)
		l = LogLevel::NFY;
	else if (l == LogLevel::NFY)
		l = LogLevel::WRN;
	else
		l = LogLevel::ERR;
	return l;
}


LogLevel &operator++(LogLevel &l)
{
	if (l == LogLevel::ERR)
		l = LogLevel::WRN;
	else if (l == LogLevel::WRN)
		l = LogLevel::NFY;
	else if (l == LogLevel::NFY)
		l = LogLevel::INF;
	else if (l == LogLevel::INF)
		l = LogLevel::DBG;
	return l;
}


Logger::Logger()
: syslog_(false),
  have_tty_(::isatty(STDERR_FILENO)),
  msg_lvl_(LogLevel::INF)
{}


Logger &Logger::instance()
{
	if (!instance_)
		instance_ = std::unique_ptr<Logger>(new Logger());
	return *instance_;
}


LogLevel &Logger::log_lvl()
{ return log_lvl_; }


Logger &flush(Logger &l)
{ return l.flush(); }


Logger &log(LogLevel lvl)
{
	Logger::instance().level(lvl);
	return Logger::instance();
}


Logger::~Logger()
{}


Logger &Logger::flush()
{
	std::lock_guard<std::mutex> l(mutex_);
	if (msg_pfx().length() == 0)
		return *this;
	if (msg_lvl_ <= log_lvl_) {
		output_message(msg_pfx());
	}
	msg_pfx() = "";

	return *this;
}


void Logger::output_message(const std::string &s)
{
	using namespace std::chrono;
	std::stringstream ss;

	if (have_tty_)
		std::cerr << color_escape(msg_lvl_);

	//auto t_now = std::time_t(duration_cast<seconds>(system_clock::now().time_since_epoch()).count());
	//ss << std::put_time(std::localtime(&t_now), /*"%Y-%m-%d "*/"%H:%M:%S");
	if (Clock::ready()) {
		ss.precision(2);
		ss << std::fixed << Clock::now().time_since_epoch().count();
		std::cerr << ss.str() << " ";
	}

	std::cerr << s << c_normal << std::endl;
}

string Logger::color_escape(LogLevel lvl)
{
	switch (lvl) {
	case LogLevel::DBG:
		return c_darkgray;
	case LogLevel::INF:
		return c_normal;
	case LogLevel::NFY:
		return c_lightgreen;
	case LogLevel::WRN:
		return c_yellow;
	case LogLevel::ERR:
		return c_lightred;
	}
	throw Bug(string(__func__) + ": Unhandled LogLevel value " + std::to_string((int)lvl));
}

std::string &Logger::msg_pfx()
{ return msg_pfx_; }



Logger &Logger::level(const LogLevel &lvl)
{
	flush();
	msg_pfx() = "";

	if (lvl == LogLevel::WRN)
		msg_pfx() += "WARNING: ";
	else if (lvl == LogLevel::ERR)
		msg_pfx() += "ERROR: ";

	this->msg_lvl_ = lvl;
	return *this;
}


Logger &Logger::operator<<( const std::string &msg)
{ msg_pfx() += msg; return *this; }

Logger &Logger::operator<< (const int i)
{ msg_pfx() += std::to_string(i); return *this; }

Logger &Logger::operator<< (const unsigned int i)
{ msg_pfx() += std::to_string(i); return *this; }

Logger &Logger::operator<< (const float &i)
{ msg_pfx() += std::to_string(i); return *this; }

Logger &Logger::operator<<(const double &d)
{ msg_pfx() += std::to_string(d); return *this; }

Logger &Logger::operator<< (const char *msg)
{ msg_pfx() += msg; return *this; }

Logger &Logger::operator<< (char *msg)
{ msg_pfx() += msg; return *this; }

Logger &Logger::operator<<(const Clock::time_point &tm)
{
	std::stringstream ss;

	ss.precision(2);
	ss << std::fixed << tm.time_since_epoch().count();
	msg_pfx() += ss.str();

	return *this;
}

Logger &Logger::operator <<(const std::thread::id &id)
{
	std::stringstream ss;
	ss << id;
	return *this << ss.str();
}

Logger &Logger::operator<< (Logger & (*pf_flush)(Logger &))
{ return pf_flush(*this); }




} // namespace gologpp
