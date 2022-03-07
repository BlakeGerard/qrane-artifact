#include "qrane_parameterlist.hh"

qrane_parameterlist::qrane_parameterlist() {
    this->params = std::vector<std::shared_ptr<qrane_parameter>>();
};

std::shared_ptr<qrane_parameter> qrane_parameterlist::at(unsigned int index) {
    return this->params.at(index);
};

void qrane_parameterlist::append(std::shared_ptr<qrane_parameter> param) {
    this->params.push_back(param);
};

unsigned int qrane_parameterlist::size() const {
    return this->params.size();
};

std::vector<std::shared_ptr<qrane_parameter>>::const_iterator
    qrane_parameterlist::cbegin() {
    return this->params.cbegin();
};

std::vector<std::shared_ptr<qrane_parameter>>::const_iterator
    qrane_parameterlist::cend() {
    return this->params.cend();
};

std::vector<std::shared_ptr<qrane_parameter>>::iterator
    qrane_parameterlist::begin() {
    return this->params.begin();
};

std::vector<std::shared_ptr<qrane_parameter>>::iterator
    qrane_parameterlist::end() {
    return this->params.end();
};

bool qrane_parameterlist::operator==(const qrane_parameterlist& rhs) {
    if (this->params.size() != rhs.params.size()) {
        return false;
    }
    for (unsigned int i = 0; i < this->params.size(); ++i) {
        if (this->params[i] != rhs.params[i]) {
            return false;
        }
    }
    return true;
};