/*
Qrane
Filename: qrane_nodelist.hh
Creation date: June 22, 2020
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

#ifndef QRANE_STATEMENTLIST_H
#define QRANE_STATEMENTLIST_H

enum decl_type {
    GATEDECL
};

class qrane_decl;

#include <memory>
#include <vector>
#include <queue>
#include "qrane_qop.hh"
#include "qrane_reg.hh"
#include "qrane_statement.hh"

typedef std::vector<std::shared_ptr<qrane_statement>>::const_iterator qrane_statementlist_const_iterator;
typedef std::vector<std::shared_ptr<qrane_statement>>::iterator qrane_statementlist_iterator;

class qrane_statementlist {

  public:
    qrane_statementlist();
    qrane_statementlist(unsigned int size);
    qrane_statementlist(qrane_statementlist_const_iterator first,
                        qrane_statementlist_const_iterator last);

    std::shared_ptr<qrane_statement> at(unsigned int index); 
    void append(std::shared_ptr<qrane_statement> statement);
    void insert(qrane_statementlist_iterator position, 
      qrane_statementlist_iterator begin, qrane_statementlist_iterator end);

    qrane_statementlist get_statements();
    qrane_statementlist get_qops();
    qrane_statementlist get_1Q_gates();
    qrane_statementlist get_2Q_gates();
    qrane_statementlist get_registers();

    bool empty() const;
    unsigned int size() const;
    void resize(unsigned int size);
    void clear();
    void reverse();
    qrane_statementlist_const_iterator cbegin() const;
    qrane_statementlist_const_iterator cend() const;
    qrane_statementlist_iterator begin();
    qrane_statementlist_iterator end();

    std::queue<std::shared_ptr<qrane_statement>> get_statements_in_queue();
  
  private:
    std::vector<std::shared_ptr<qrane_statement>> statements;
};

#endif
