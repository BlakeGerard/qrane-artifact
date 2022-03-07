#include "qrane_qop.hh"
#include "qrane_argumentlist.hh"
#include <sstream>

qrane_qop::qrane_qop() : qrane_statement(statement_type::QOP) {
    this->type = qop_type::GATE;
    this->id = -1;
    this->operation = std::string("default");
};

qrane_qop::qrane_qop(statement_type statement_type, qop_type qop_type, 
    qop_id id, std::string operation) : qrane_statement(statement_type) {
    this->type = qop_type;
    this->id = id;
    this->operation = id;
};

qrane_qop::qrane_qop(statement_type statement_type, qop_type qop_type, 
    qop_id id, std::string operation, 
    qrane_argumentlist args) : qrane_statement(statement_type) {
    this->type = qop_type;
    this->id = id;
    this->operation = operation;
    this->params = qrane_parameterlist();
    this->args = args;
};

qrane_qop::qrane_qop(statement_type statement_type, qop_type qop_type, 
    qop_id id, std::string operation,
    qrane_parameterlist params, qrane_argumentlist args) : qrane_statement(statement_type) {
    this->type = qop_type;
    this->id = id;
    this->operation = operation;
    this->params = params;
    this->args = args;
};

qop_id qrane_qop::get_id() {
    return this->id;
};

std::string qrane_qop::get_operation() {
    return this->operation;
};

unsigned int qrane_qop::num_params() const {
    return this->params.size();
};

unsigned int qrane_qop::num_args() const {
    return this->args.size();
};

std::shared_ptr<qrane_parameter> qrane_qop::param(unsigned int index) {
    return this->params.at(index);
};

std::shared_ptr<qrane_argument> qrane_qop::arg(unsigned int index) {
    return this->args.at(index);
};

qrane_parameterlist qrane_qop::get_parameters() {
    return this->params;
};  

qrane_argumentlist qrane_qop::get_arguments() {
    return this->args;
};

bool qrane_qop::operator==(const qrane_qop& rhs) {
    bool c0 = this->type == rhs.type;
    bool c1 = this->operation == rhs.operation;
    bool c2 = this->params == rhs.params;
    bool c3 = this->args == rhs.args;
    return c0 && c1 && c2 && c3;
};

std::shared_ptr<qrane_qop> qrane_qop::make_unassigned_2Q_gate(std::string operation, 
    std::string reg_name, qubit_id arg0, qubit_id arg1) {
    auto qarg0 = std::make_shared<qrane_argument>(reg_name, arg0);
    auto qarg1 = std::make_shared<qrane_argument>(reg_name, arg1);
    qrane_argumentlist args = qrane_argumentlist({qarg0, qarg1});
    qrane_qop ret = qrane_qop(statement_type::QOP, qop_type::GATE, 
                              -1, operation, args);
    return std::make_shared<qrane_qop>(ret);
};

bool qrane_qop::is_1Q_gate() {
    return this->args.size() == 1;
};

bool qrane_qop::is_2Q_gate() {
    return this->args.size() == 2;
};

std::string qrane_qop::to_string() {
    std::ostringstream strm;
    unsigned int ctr;

    strm << operation;
    if (num_params()) {
        ctr = 0;
        strm << "(";
        for (auto& param : params) {
            strm << param->to_string();
            if (ctr != num_params() - 1) { strm << ", "; }
            ++ctr;
        }
        strm << ")";
    }
    strm << " ";
    if (num_args()) {
        ctr = 0;
        for (auto& arg : args) {
            strm << arg->to_string();
            if (ctr != num_args() - 1) { strm << ", "; }
            ++ctr;
        }
        strm << ";";
    }
    return strm.str();
};