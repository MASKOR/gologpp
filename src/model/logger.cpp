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

#include <iostream>
#include <iomanip>



namespace gologpp {


std::unique_ptr<Logger> Logger::instance_;
thread_local std::unique_ptr<std::string> Logger::msg_pfx_(new std::string());


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
  log_lvl_(LogLevel::INF),
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
	if (msg_pfx().length() == 0) return *this;
	if (msg_lvl_ <= log_lvl_) {
		output_message(msg_pfx());
	}
	msg_pfx() = "";

	return *this;
}


void Logger::output_message(const std::string &s)
{
	std::cerr << s << std::endl;
}

std::string &Logger::msg_pfx()
{ return *msg_pfx_; }



Logger &Logger::level(const LogLevel &lvl)
{
	flush();
	if (!syslog_ && msg_lvl_ != lvl && lvl >= log_lvl_ && msg_lvl_ >= log_lvl_)
		msg_pfx() = "\n";
	else
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

Logger &Logger::operator<< (const char *msg)
{ msg_pfx() += msg; return *this; }

Logger &Logger::operator<< (char *msg)
{ msg_pfx() += msg; return *this; }

Logger &Logger::operator<<(const Clock::time_point &tm)
{
	using namespace std::chrono;
	std::stringstream ss;
	ss.precision(2);

	/*auto sys_now = system_clock::now();
	auto delta = tm - Clock::now();
	auto sys_tm = sys_now + delta;
	auto in_time_t = std::time_t(duration_cast<seconds>(sys_tm.time_since_epoch()).count());//*/

	//ss << std::put_time(std::localtime(&in_time_t), /*"%Y-%m-%d "*/"%H:%M:%S");
	ss << std::fixed << tm.time_since_epoch().count();

	msg_pfx() += ss.str();
	return *this;
}

Logger &Logger::operator<< (Logger & (*pf_flush)(Logger &))
{ return pf_flush(*this); }




} // namespace gologpp
