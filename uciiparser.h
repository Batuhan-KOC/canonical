#ifndef UCIIPARSER_H
#define UCIIPARSER_H

#include "canonicalinterface.h"

#define sha_key "sha"

class UCIIParser : public CanonicalInterface
{
public:
    UCIIParser();
    virtual ~UCIIParser();

    enum OperationType{
        Undefined,
        AllSupportedUbuntuRelases,
        CurrentUbuntuLTSVersion,
        FetchSha256
    };

    virtual int requestOperation(boost::any operationType, boost::program_options::variables_map& args = Hidden::AVAL) override;

private:
    bool obtainJsonFile();

    int doOperationAllSupportedUbuntuReleases();
    int doOperationCurrentUbuntuLTSVersion();
    int doOperationFetchSha256(std::string releaseName);
};

#endif // UCIIPARSER_H
