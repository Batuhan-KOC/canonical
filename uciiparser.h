#ifndef UCIIPARSER_H
#define UCIIPARSER_H

#include "canonicalinterface.h"
#include <json/json.h>

#define sha_key "sha"
#define release_title_key "release_title"
#define release_codename_key "release_codename"
#define version_key "version"

class UCIIParser : public CanonicalInterface
{
public:
    UCIIParser();
    virtual ~UCIIParser();

    enum OperationType{
        Undefined,
        AllSupportedUbuntuRelases,
        CurrentUbuntuLTSVersion,
        FetchSha256,
        ListVersions
    };

    virtual int requestOperation(boost::any operationType, boost::program_options::variables_map& args = Hidden::AVAL) override;

private:
    // Fetched json data from the url
    Json::Value jsonData;

    bool obtainJsonFile();

    int doOperationAllSupportedUbuntuReleases();
    int doOperationCurrentUbuntuLTSVersion();
    int doOperationFetchSha256(std::string releaseTitle, std::string releaseCodename, std::string version);
    int doOperationFindVersions(std::string releaseTitle, std::string releaseCodename, bool bypassHeadingText = false);
};

#endif // UCIIPARSER_H
