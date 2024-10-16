#ifndef UCIIPARSER_H
#define UCIIPARSER_H

#include "canonicalinterface.h"

class UCIIParser : public CanonicalInterface
{
public:
    UCIIParser();
    virtual ~UCIIParser();

    enum OperationType{
        AllSupportedUbuntuRelases,
        CurrentUbuntuLTSVersion,
        FetchSha256
    };

    virtual int requestOperation(boost::any operationType, boost::program_options::variables_map args) override;

protected:
    virtual int doOperation(boost::any& operationType, boost::program_options::variables_map& args) override;

};

#endif // UCIIPARSER_H
