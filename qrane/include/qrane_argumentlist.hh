/*
Qrane
Filename: qrane_argumentlist.hh
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

#ifndef QRANE_ARGUMENTLIST_H
#define QRANE_ARGUMENTLIST_H

#include <vector>
#include <string>
#include <iostream>
#include <initializer_list>
#include "qrane_argument.hh"

class qrane_argumentlist {

    public:
        qrane_argumentlist();
        qrane_argumentlist(std::initializer_list<std::shared_ptr<qrane_argument>> args);
        std::shared_ptr<qrane_argument> at(unsigned int index);
        std::shared_ptr<qrane_argument> operator[](unsigned int index) const;
        void append(std::shared_ptr<qrane_argument> arg);
        unsigned int size() const;
        std::vector<std::shared_ptr<qrane_argument>>::const_iterator cbegin();
        std::vector<std::shared_ptr<qrane_argument>>::const_iterator cend();
        std::vector<std::shared_ptr<qrane_argument>>::iterator begin();
        std::vector<std::shared_ptr<qrane_argument>>::iterator end();

        bool operator==(const qrane_argumentlist& rhs);

    private:
        std::vector<std::shared_ptr<qrane_argument>> args;
};

#endif