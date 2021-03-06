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


#include "model_element.h"
#include "semantics.h"

namespace gologpp {


string ModelElement::str() const
{ return to_string(""); }

void ModelElement::set_semantics(std::unique_ptr<GeneralSemantics<ModelElement>> &&impl)
{ semantics_ = std::move(impl); }



/*GeneralSemantics<ModelElement>::GeneralSemantics()
{}*/

GeneralSemantics<ModelElement>::~GeneralSemantics()
{}


}
