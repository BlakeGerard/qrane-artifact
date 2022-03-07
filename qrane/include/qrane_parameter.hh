/*
Qrane
Filename: qrane_parameter.hh
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

#ifndef QRANE_PARAMETER_H
#define QRANE_PARAMETER_H

#include <memory>
#include <string>
#include <sstream>

enum binaryop_type {
  ADD,
  SUB,
  MUL,
  DIV,
  CAR
};

enum unaryop_type {
  SIN,
  COS,
  TAN,
  EXP,
  LN,
  SQRT,
  NEG,
  PAR
};

class qrane_parameter {

  public:
    virtual std::string to_string() = 0;
};

template <class T>
class qrane_value : public qrane_parameter {

  protected:
    T value;

  public:
    qrane_value(T value);
    std::string to_string();
};

class qrane_binaryop : public qrane_parameter {

  protected:
    binaryop_type type;
    std::string opt;
    std::shared_ptr<qrane_parameter> lhs;
    std::shared_ptr<qrane_parameter> rhs;
  
  public:
    qrane_binaryop(binaryop_type type, std::string opt, std::shared_ptr<qrane_parameter> lhs, std::shared_ptr<qrane_parameter> rhs);
    void set_binaryop_type(binaryop_type type);
    void set_lhs(std::shared_ptr<qrane_parameter> lhs);
    void set_rhs(std::shared_ptr<qrane_parameter> rhs);
    std::string to_string();
};

class qrane_unaryop : public qrane_parameter {

  protected:
    unaryop_type type;
    std::string opt;
    std::shared_ptr<qrane_parameter> arg;

  public:
    qrane_unaryop(unaryop_type type, std::string opt, std::shared_ptr<qrane_parameter> arg);
    void set_unaryop_type(unaryop_type type);
    void set_arg(std::shared_ptr<qrane_parameter> arg);
    std::string to_string();
};

#endif