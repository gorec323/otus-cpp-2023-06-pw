#include "route.hpp"

namespace netlib {

Route::Route(std::string nodeName):
    m_nodeName {std::move(nodeName)}
{
}

}