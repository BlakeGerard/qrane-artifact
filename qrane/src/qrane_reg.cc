/*
Qrane
Filename: qrane_reg.cc
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

#include "qrane_reg.hh"
#include <fstream>

qrane_reg::qrane_reg() : qrane_statement(statement_type::REG) {
    this->type = reg_type::QREG;
    this->device = std::string("qreg");
    this->name = std::string("default");
};

/*
    Construct an empty register.
*/
qrane_reg::qrane_reg(statement_type statement_type, reg_type reg_type, 
    std::string device, std::string name) : qrane_statement(statement_type) {
    this->type = reg_type;
    this->device = device;
    this->name = name;
    this->reg = std::map<qubit_id, qubit_id>();
};

/*
    Construct a register populated with indices
    in the range [0, size - 1].
*/
qrane_reg::qrane_reg(statement_type statement_type, reg_type reg_type, 
    std::string device, std::string name, unsigned int size) : qrane_statement(statement_type) {
    this->type = reg_type;
    this->device = device;
    this->name = name;
    this->reg = std::map<qubit_id, qubit_id>();

    qubit_id id = 0;
    for (; id < size;) {
        this->reg.insert(std::make_pair(id, id));
        ++id;
    }
};

/*
    Create a register from an initializer_list.
*/
qrane_reg::qrane_reg(statement_type statement_type, reg_type reg_type,
    std::string device, std::string name, 
    std::initializer_list<qubit_id> reg) : qrane_statement(statement_type) {
    this->type = reg_type;
    this->device = device;
    this->name = name;
    this->reg = std::map<qubit_id, qubit_id>();

    for (const auto& id : reg) {
        this->reg.insert(std::make_pair(id, id));
    }
};

/*
    Construct a register populated with indices
    in the range [incl_start, incl_stop].
*/
qrane_reg::qrane_reg(statement_type statement_type, reg_type reg_type, 
    std::string device, std::string name, 
    qop_id incl_start, qop_id incl_stop) : qrane_statement(statement_type) {
    this->type = reg_type;
    this->device = device;
    this->name = name;
    this->reg = std::map<qubit_id, qubit_id>();

    qubit_id id = incl_start;
    for (; id <= incl_stop;) {
        this->reg.insert(std::make_pair(id, id));
        ++id;
    }
}

qrane_reg qrane_reg::trivial_layout(unsigned int size) {
    return qrane_reg(statement_type::REG, reg_type::QREG, "qreg", "q", size);
};

void qrane_reg::insert(qubit_id id) {
    this->reg.insert(std::make_pair(id, id));
}

void qrane_reg::insert(qubit_id physical, qubit_id logical) {
    this->reg.insert(std::make_pair(physical, logical));
}

qubit_id qrane_reg::at(qubit_id id) {
    return this->reg.at(id);
};

std::string qrane_reg::get_name() {
    return this->name;
};

unsigned int qrane_reg::size() const {
    return this->reg.size();
};

std::string qrane_reg::to_string() {
    std::ostringstream strm;
    strm << device << " " << name << "[" << this->reg.size() << "];";
    return strm.str();
};

bool qrane_reg::count(qubit_id id) {
    return this->reg.count(id);
};

qrane_reg_iterator qrane_reg::begin() {
    return this->reg.begin();
};

qrane_reg_iterator qrane_reg::end() {
    return this->reg.end();
};

qrane_reg_const_iterator qrane_reg::begin() const {
    return this->reg.cbegin();
};

qrane_reg_const_iterator qrane_reg::end() const {
    return this->reg.cend();
};

bool qrane_reg::is_qreg() {
    return this->type == reg_type::QREG;
};

bool qrane_reg::is_creg() {
    return this->type == reg_type::CREG;
};

qubit_id qrane_reg::operator[](unsigned int index) const {
    return this->reg.at(index);
};

bool qrane_reg::operator==(const qrane_reg& rhs) const {
    for (const auto& entry : this->reg) {
        if (!rhs.reg.count(entry.first)) {
            return false;
        }
        if (entry.second != rhs.reg.at(entry.first)) {
            return false;
        }
    }
    return this->type == rhs.type;
};

void qrane_reg::print_current_mapping() {
    std::ostringstream strm;
    strm << "{ ";
    for (const auto& entry : this->reg) {
        strm << "q[" << entry.first << "] -> l[" << entry.second << "], ";
    }
    strm.seekp(-2, std::ios_base::end); // Revert before the trailing ','
    strm << "}" << std::endl;
    std::cout << strm.str();
};

void qrane_reg::permute_register(std::shared_ptr<qrane_qop> swap) {
    qubit_id lhs = swap->arg(0)->get_index();
    qubit_id rhs = swap->arg(1)->get_index();
    qubit_id temp;
    if (this->reg.count(lhs) && this->reg.count(rhs)) {
        temp = this->reg[lhs];
        this->reg[lhs] = this->reg[rhs];
        this->reg[rhs] = temp;
    }
};

qrane_reg qrane_reg::read_from_edge_list_file(std::string mapping_path) {
    qrane_reg ret = qrane_reg();
    std::ifstream in_strm;
    std::string line, lhs, rhs;

    in_strm.open(mapping_path, std::ios_base::in);
    if (in_strm.is_open()) {
        while(std::getline(in_strm, line)) {
            if (line == "") { continue; }
            auto space = line.find(" ");
            lhs = line.substr(0, space);
            rhs = line.substr(space+1);
            auto lhs_id = std::stoi(lhs);
            auto rhs_id = std::stoi(rhs);
            ret.insert(lhs_id, rhs_id);
        }
        in_strm.close();
    } else {
        std::cout << "Unable to open mapping file. Returning empty qrane_reg.\n";
    }

    return ret;
};