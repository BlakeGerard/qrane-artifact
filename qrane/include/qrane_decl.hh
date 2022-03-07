/*
Qrane
Filename: qrane_decl.hh
Creation date: June 30, 2020
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

#ifndef QRANE_DECL_H
#define QRANE_DECL_H

#include "qrane_statement.hh"
#include "qrane_parameterlist.hh"
#include "qrane_argumentlist.hh"
#include "qrane_statementlist.hh"

class qrane_decl : public qrane_statement {

    public:
        qrane_decl(statement_type statement_type, decl_type decl_type, std::string operation);
        qrane_decl(statement_type statement_type, decl_type decl_type, std::string operation,
            qrane_argumentlist args);
        qrane_decl(statement_type statement_type, decl_type decl_type, std::string operation,
            qrane_parameterlist params, qrane_argumentlist args);
        std::string get_operation();
        void set_statementlist(qrane_statementlist statements);
        std::string to_string();

    private:
        decl_type type;
        std::string operation;
        qrane_parameterlist params;
        qrane_argumentlist args;
        qrane_statementlist statements;
};

#endif