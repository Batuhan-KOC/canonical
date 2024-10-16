/**
 * @file uciiparser.h
 * @brief This header file contains the declarations of the ubuntu cloud image information (ucii) derived class and its methods
 * @author Batuhan KOÇ
 * @date 2024-10-16
 */

#ifndef UCIIPARSER_H
#define UCIIPARSER_H

#include "canonicalinterface.h"
#include <json/json.h>

#define sha_key "sha"
#define release_title_key "release_title"
#define release_codename_key "release_codename"
#define version_key "version"

/**
 * @class UCIIParser
 * @brief This class represents a commandline interface parsing class that performs several operations on ubuntu 
 */
class UCIIParser : public CanonicalInterface
{
public:
    /**
     * @brief UCII class default constructor
    */
    UCIIParser();
    /**
     * @brief UCII class default destructor
    */
    virtual ~UCIIParser();

    /**
     * @enum Command line interface available operations enumeration list
    */
    enum OperationType{
        Undefined,
        AllSupportedUbuntuRelases,
        CurrentUbuntuLTSVersion,
        FetchSha256,
        ListVersions
    };

    /**
     * @brief UCIIParser accepts operation requests and their relevant input arguments via requestOperation method.
     * @param operationType is the type of the operation requested via cli.
     * @param args list of arguments related to the type of operation requested via cli. In default, a random reference value is assigned.
    */
    virtual int requestOperation(boost::any operationType, boost::program_options::variables_map& args = Hidden::AVAL) override;

private:
    // Fetched json data from the url
    Json::Value jsonData;

    /**
     * @brief reads the json file from the url and stores it into jsonData parameter.
    */
    bool obtainJsonFile();

    /**
     * @brief parse all of the amd64 architecture ubuntu release versions and print them line by line.
    */
    int doOperationAllSupportedUbuntuReleases();

    /**
     * @brief parse the last released amd64 architecture LTS ubuntu version and print it.
    */
    int doOperationCurrentUbuntuLTSVersion();

    /**
     * @brief parse the all amd64 architecture ubuntu releases by release title or release codename and prints the 
     * sha64 number of the disk1.img item of the given version.
     * @param releaseTitle requested ubuntu version's release title
     * @param releaseCodename requested ubuntu version's codename
     * @param version requested version of the given ubuntu release
    */
    int doOperationFetchSha256(std::string releaseTitle, std::string releaseCodename, std::string version);
    
    /**
     * @brief fetch the all amd64 architecture ubuntu release versions and filter them by the release title or codename
     * and print available version numbers based on the given arguments.
     * @param releaseTitle requested ubuntu release's title
     * @param releaseCodename requested ubuntu release's codename
     * @param bypassHeadingText if true, does not display the informative text displayed on the command line at first.
    */
    int doOperationFindVersions(std::string releaseTitle, std::string releaseCodename, bool bypassHeadingText = false);
};

#endif // UCIIPARSER_H
