/**
 * @file canonicalinterface.h
 * @brief This header file contains the declarations of canonical interface class and its methods.
 * @author Batuhan KOÇ
 * @date 2024-10-16
 */

#ifndef CANONICALINTERFACE_H
#define CANONICALINTERFACE_H

#include <iostream>

#include <boost/program_options.hpp>

/**
 * @namespace Hidden
 * @brief Contains the default referancable variable to be able to call the requestOperation function with single argument.
*/
namespace Hidden{
    static boost::program_options::variables_map AVAL = boost::program_options::variables_map();
}

/**
 * @class CanonicalInterface
 * @brief This class represents a generic interface class that aims to be a base for constructing a cli interface sub-class
 */
class CanonicalInterface
{
public:
    /**
     * @brief CLI request handling base class method.
     * @param operationType The type of the operation requested from command line interface
     * @param args The additional arguments related to the operation type.
     */
    virtual int requestOperation(boost::any operationType, boost::program_options::variables_map& args = Hidden::AVAL) = 0;
};

#endif // CANONICALINTERFACE_H
