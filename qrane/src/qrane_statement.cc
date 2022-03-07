/*
Qrane
Filename: qrane_statement.cc
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

#include "qrane_statement.hh"

qrane_statement::qrane_statement(statement_type type) {
    this->type = type;
};

statement_type qrane_statement::get_statement_type() const {
    return this->type;
};

bool qrane_statement::is_decl() const {
    return this->type == statement_type::DECL;
};

bool qrane_statement::is_reg() const {
    return this->type == statement_type::REG;
};

bool qrane_statement::is_qop() const {
    return this->type == statement_type::QOP;
};