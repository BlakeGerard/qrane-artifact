/*
Qrane
Filename: qrane_argument.cc
Creation date: July 2, 2020
Copyright (C) 2020

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qrane_argument.hh"

qrane_argument::qrane_argument() {
    this->reg = std::string("");
    this->index = 999999;
};

qrane_argument::qrane_argument(std::string reg) {
    this->reg = reg;
    this->index = 999999;
};

qrane_argument::qrane_argument(std::string reg, qubit_id index) {
    this->reg = reg;
    this->index = index;
};

std::string qrane_argument::get_reg() const {
    return this->reg;
};

qubit_id qrane_argument::get_index() const {
    return this->index;
};

std::string qrane_argument::to_string() const {
    std::ostringstream strm;
    strm << reg << "[" << index << "]";
    return strm.str();
};