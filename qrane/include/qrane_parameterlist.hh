#ifndef QRANE_PARAMETERLIST_H
#define QRANE_PARAMETERLIST_H

#include <memory>
#include <vector>
#include "qrane_parameter.hh"

class qrane_parameterlist {
    public:
        qrane_parameterlist();
        std::shared_ptr<qrane_parameter> at(unsigned int index);
        void append(std::shared_ptr<qrane_parameter> param);
        unsigned int size() const;
        std::vector<std::shared_ptr<qrane_parameter>>::const_iterator cbegin();
        std::vector<std::shared_ptr<qrane_parameter>>::const_iterator cend();
        std::vector<std::shared_ptr<qrane_parameter>>::iterator begin();
        std::vector<std::shared_ptr<qrane_parameter>>::iterator end();

        bool operator==(const qrane_parameterlist& rhs);

    private:
        std::vector<std::shared_ptr<qrane_parameter>> params;
};

#endif