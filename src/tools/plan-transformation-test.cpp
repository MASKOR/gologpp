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
 * along with golog++.	If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include <memory>
#include <iostream>

#include <model/platform_model.h>
#include <parser/parser.h>
#include <parser/platform_parser.h>


using namespace gologpp;


void test_parser(const string &filename, const string &pm_filename)
{

	unique_ptr<PlatformModel> v = parser::parse_platform_model(pm_filename);
	vector<shared_ptr<PlatformConstraint>> v1 = parser::parse_constraints(filename);
}



int main(int argc, const char **argv)
{
	string filename = SOURCE_DIR "/examples/constraints.gpp";
	string pm_filename = SOURCE_DIR "/examples/platform.gpp";
	if (argc > 1)
		filename = argv[1];
	test_parser(filename, pm_filename);
	return 0;
}





