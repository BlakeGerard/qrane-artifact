/*
Qrane
Filename: qrane_decl.cc
Creation date: July 1, 2020
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

#include "qrane_decl.hh"

qrane_decl::qrane_decl(statement_type statement_type, decl_type decl_type, 
    std::string operation) : qrane_statement(statement_type) {
    this->type = decl_type;
    this->operation = operation;
    this->params = qrane_parameterlist();
    this->args = qrane_argumentlist();
    this->statements = qrane_statementlist();
};

qrane_decl::qrane_decl(statement_type statement_type, decl_type decl_type, 
    std::string operation, qrane_argumentlist args) : qrane_statement(statement_type) {
    this->type = decl_type;
    this->operation = operation;
    this->params = qrane_parameterlist();
    this->args = args;
    this->statements = qrane_statementlist();
};

qrane_decl::qrane_decl(statement_type statement_type, decl_type decl_type, 
    std::string operation, qrane_parameterlist params, 
    qrane_argumentlist args) : qrane_statement(statement_type) {
    this->type = decl_type;
    this->operation = operation;
    this->params = params;
    this->args = args;
    this->statements = qrane_statementlist();
};

void qrane_decl::set_statementlist(qrane_statementlist statements) {
    this->statements = statements;
}

std::string qrane_decl::get_operation() {
    return this->operation;
};

std::string qrane_decl::to_string() {
    return std::string("decl");
};