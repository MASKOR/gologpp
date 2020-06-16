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

#include <memory>


namespace gologpp {


enum class LogLevel {
	ERR,
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
	static std::unique_ptr<Logger> instance_;
public:
	~Logger();
	Logger &level(const LogLevel &lvl);
	Logger &flush();
	static Logger &instance();
	LogLevel &log_lvl();

	Logger &operator<< (const std::string &msg);
	Logger &operator<< (const unsigned int i);
	Logger &operator<< (const int i);
	Logger &operator<< (const float &d);
	Logger &operator<< (Logger & (*pf_flush)(Logger &));
	Logger &operator<< (const char *msg);
	Logger &operator<< (char *msg);

	template<class ListT>
	Logger &operator<< (const ListT &l) {
		msg_pfx_ += "(";
		for (auto elem : l) {
			msg_pfx_ += std::to_string(elem) + ", ";
		}
		msg_pfx_.pop_back(); msg_pfx_.pop_back();
		msg_pfx_ += ")";
		return *this;
	}

	virtual void output_message(const std::string &msg);

private:
	bool syslog_;
	LogLevel log_lvl_;
	LogLevel msg_lvl_;
	std::string msg_pfx_;
	std::exception_ptr exception_;
};

Logger &flush(Logger &l);
Logger &log(LogLevel lvl);


} // namespace gologpp
