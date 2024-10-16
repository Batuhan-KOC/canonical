#ifndef CANONICALINTERFACE_H
#define CANONICALINTERFACE_H

#include <iostream>

#include <boost/program_options.hpp>

namespace Hidden{
    static boost::program_options::variables_map AVAL = boost::program_options::variables_map();
}

class CanonicalInterface
{
public:
    virtual int requestOperation(boost::any operationType, boost::program_options::variables_map& args = Hidden::AVAL) = 0;
};

#endif // CANONICALINTERFACE_H
