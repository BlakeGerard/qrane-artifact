/*
Qrane
Filename: qrane_qop.hh
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

#ifndef QRANE_QOP_H
#define QRANE_QOP_H

#include "qrane_shared.hh"
#include "qrane_statement.hh"
#include "qrane_argumentlist.hh"
#include "qrane_parameterlist.hh"

enum qop_type {
    GATE,
    MEASURE,
    RESET,
    OPAQUE,
    BARRIER
};

class qrane_qop : public qrane_statement {

    public:
        qrane_qop();
        qrane_qop(statement_type statement_type, qop_type qop_type, 
            qop_id id, std::string operation);
        qrane_qop(statement_type statement_type, qop_type type, 
            qop_id id, std::string operation,
            qrane_argumentlist args);
        qrane_qop(statement_type statement_type, qop_type qop_type, 
            qop_id  id, std::string operation,
            qrane_parameterlist params, qrane_argumentlist args);

        std::shared_ptr<qrane_parameter> param(unsigned int index);
        std::shared_ptr<qrane_argument> arg(unsigned int index);
        unsigned int num_params() const;
        unsigned int num_args() const;
        qrane_parameterlist get_parameters();
        qrane_argumentlist get_arguments();

        static std::shared_ptr<qrane_qop> make_unassigned_2Q_gate(std::string operation, 
            std::string reg_name, qubit_id arg0, qubit_id arg1);

        qop_id get_id();
        std::string get_operation();
        std::string to_string();
        bool is_1Q_gate();
        bool is_2Q_gate();

        bool operator==(const qrane_qop& rhs);

    private:
        qop_type type;
        qop_id  id;
        std::string operation;
        qrane_parameterlist params;
        qrane_argumentlist args;
};

#endif