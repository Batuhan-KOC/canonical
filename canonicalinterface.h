#ifndef CANONICALINTERFACE_H
#define CANONICALINTERFACE_H

#include <boost/program_options.hpp>

class CanonicalInterface
{
public:
    virtual int requestOperation(boost::any operationType, boost::program_options::variables_map args) = 0;

protected:
    virtual int doOperation(boost::any& operationType, boost::program_options::variables_map& args) = 0;

};

#endif // CANONICALINTERFACE_H
