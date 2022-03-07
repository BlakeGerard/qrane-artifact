/*
Qrane
Filename: qrane_parameter.cc
Creation date: June 28, 2020
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

#include "qrane_parameter.hh"
#include <string>

template <class T>
qrane_value<T>::qrane_value(T value) {
    this->value = value;
};

template <class T>
std::string qrane_value<T>::to_string() {
    std::ostringstream strm;
    strm << value;
    return strm.str();
};

template class qrane_value<double>;
template class qrane_value<int>;
template class qrane_value<std::string>;

qrane_binaryop::qrane_binaryop(binaryop_type type, std::string opt,
    std::shared_ptr<qrane_parameter> lhs, std::shared_ptr<qrane_parameter> rhs) {
    this->type = type;
    this->opt = opt;
    this->lhs = lhs;
    this->rhs = rhs;
};

void qrane_binaryop::set_binaryop_type(binaryop_type type) {
    this->type = type;
};

void qrane_binaryop::set_lhs(std::shared_ptr<qrane_parameter> lhs) {
    this->lhs = lhs;
};

void qrane_binaryop::set_rhs(std::shared_ptr<qrane_parameter> rhs) {
    this->rhs = rhs;
};

std::string qrane_binaryop::to_string() {
    std::ostringstream strm;
    strm << lhs->to_string() << " " << opt << " " << rhs->to_string();
    return strm.str();
};

qrane_unaryop::qrane_unaryop(unaryop_type type, std::string opt, std::shared_ptr<qrane_parameter> arg) {
    this->type = type;
    this->opt = opt;
    this->arg = arg;
};

void qrane_unaryop::set_unaryop_type(unaryop_type type) {
    this->type = type;
};

void qrane_unaryop::set_arg(std::shared_ptr<qrane_parameter> arg) {
    this->arg = arg;
};

std::string qrane_unaryop::to_string() {
    std::ostringstream strm;
    strm << opt << " " << arg->to_string();
    return strm.str();
};