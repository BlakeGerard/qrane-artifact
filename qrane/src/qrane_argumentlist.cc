/*
Qrane
Filename: qrane_argumentlist.cc
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

#include "qrane_argumentlist.hh"

qrane_argumentlist::qrane_argumentlist() {
    this->args = std::vector<std::shared_ptr<qrane_argument>>();
};

qrane_argumentlist::qrane_argumentlist(
    std::initializer_list<std::shared_ptr<qrane_argument>> args) {
    this->args = std::vector<std::shared_ptr<qrane_argument>>(args);
};

std::shared_ptr<qrane_argument> qrane_argumentlist::at(unsigned int index) {
    return this->args.at(index);
}

void qrane_argumentlist::append(std::shared_ptr<qrane_argument> arg) {
    this->args.push_back(arg);
};

unsigned int qrane_argumentlist::size() const {
    return this->args.size();
}

std::vector<std::shared_ptr<qrane_argument>>::const_iterator 
    qrane_argumentlist::cbegin() {
    return this->args.cbegin();
};

std::vector<std::shared_ptr<qrane_argument>>::const_iterator 
    qrane_argumentlist::cend() {
    return this->args.cend();
};

std::vector<std::shared_ptr<qrane_argument>>::iterator 
    qrane_argumentlist::begin() {
    return this->args.begin();
};

std::vector<std::shared_ptr<qrane_argument>>::iterator 
    qrane_argumentlist::end() {
    return this->args.end();
};

bool qrane_argumentlist::operator==(const qrane_argumentlist& rhs) {
    if (this->args.size() != rhs.args.size()) {
        return false;
    }
    for (unsigned int i = 0; i < this->args.size(); ++i) {
        if (this->args[i] != rhs.args[i]) {
            return false;
        }
    }
    return true;
};
