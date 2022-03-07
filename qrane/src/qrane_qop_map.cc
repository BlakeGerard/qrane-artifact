#include "qrane_qop_map.hh"

qrane_qop_map::qrane_qop_map() {

};

qrane_qop_map::qrane_qop_map(qrane_statementlist qops) {
    this->map = std::map<unsigned int, std::shared_ptr<qrane_qop>>();
    for (const auto& entry : qops) {
        std::shared_ptr<qrane_qop> qop = std::dynamic_pointer_cast<qrane_qop>(entry);
        this->map.insert(std::make_pair(qop->get_id(), qop));
    }
};

std::shared_ptr<qrane_qop> qrane_qop_map::operator[](unsigned int index) const {
    return this->map.at(index);
};

unsigned int qrane_qop_map::size() const {
    return this->map.size();
};

std::map<unsigned int, std::shared_ptr<qrane_qop>>::const_iterator 
    qrane_qop_map::begin() const {
    return this->map.begin();
};

std::map<unsigned int, std::shared_ptr<qrane_qop>>::const_iterator 
    qrane_qop_map::end() const {
    return this->map.end();
};