/*
    File: qrane_qop_map.hh
    Author: Blake Gerard
    Created: 10/28/21
    Description:
        Dedicated class to wrap map of id -> shared_ptr<qrane_qop>.
        THE map CONTAINER MUST BE ORDERED.
*/

#include <map>
#include "qrane_statementlist.hh"

class qrane_qop_map {

    public:
        qrane_qop_map();
        qrane_qop_map(qrane_statementlist qops);
        std::shared_ptr<qrane_qop> operator[](unsigned int) const;
        unsigned int size() const;
        std::map<unsigned int, std::shared_ptr<qrane_qop>>::const_iterator begin() const;
        std::map<unsigned int, std::shared_ptr<qrane_qop>>::const_iterator end() const;

    private:
        std::map<unsigned int, std::shared_ptr<qrane_qop>> map;
};