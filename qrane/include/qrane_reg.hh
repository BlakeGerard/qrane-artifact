/*
    Project: qrane
    Filename: qrane_reg.hh
    Author: Blake Gerard
    Description:
        A qrane_reg represents a register of physical (qu)bits mapped to their
        logical counterparts. Internally, an instance of qrane_reg maintains
        an std::map from qubit_id -> qubit_id, where the keys are physical
        qubits in the device and values are their corresponding logical qubits.
        This is done to make register permutation easy.
*/

#ifndef QRANE_DECL
#define QRANE_DECL

#include "qrane_qop.hh"
#include "qrane_statement.hh"
#include "qrane_shared.hh"
#include <sstream>
#include <map>

enum reg_type {
    QREG,
    CREG,
};

typedef std::map<qubit_id, qubit_id>::iterator qrane_reg_iterator;
typedef std::map<qubit_id, qubit_id>::const_iterator qrane_reg_const_iterator;

class qrane_reg : public qrane_statement {

    public: 
        qrane_reg();
        qrane_reg(statement_type statement_type, reg_type reg_type, 
            std::string device, std::string name);
        qrane_reg(statement_type statement_type, reg_type reg_type, 
            std::string device, std::string name, unsigned int size);
        qrane_reg(statement_type statement_type, reg_type reg_type,
            std::string device, std::string name, qubit_id start, qubit_id stop);
        qrane_reg(statement_type statement_type, reg_type reg_type,
            std::string device, std::string name, std::initializer_list<qubit_id> reg);

        void insert(qubit_id id);
        void insert(qubit_id physical, qubit_id logical);
        qubit_id at(qubit_id);
        std::string get_name();
        unsigned int size() const;
        std::string to_string();
        bool count(qubit_id id);
        bool is_qreg();
        bool is_creg();
        qrane_reg_iterator begin();
        qrane_reg_iterator end();
        qrane_reg_const_iterator begin() const;
        qrane_reg_const_iterator end() const;

        void print_current_mapping();
        void permute_register(std::shared_ptr<qrane_qop> swap);
        qubit_id operator[](unsigned int index) const;
        bool operator==(const qrane_reg& rhs) const;

        static qrane_reg trivial_layout(unsigned int size);
        static qrane_reg read_from_edge_list_file(std::string mapping_path);

    private:
        reg_type type;
        std::string device;
        std::string name;
        std::map<qubit_id, qubit_id> reg;
};

#endif