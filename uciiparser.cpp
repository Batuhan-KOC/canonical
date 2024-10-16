#include "uciiparser.h"

#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string>

#define LTS_keyword "LTS"

UCIIParser::UCIIParser() {

}

UCIIParser::~UCIIParser()
{

}

int UCIIParser::requestOperation(boost::any operationType, boost::program_options::variables_map& args)
{
    // Perform prechecks
    int retVal = 0;

    OperationType opType = boost::any_cast<OperationType>(operationType);
    switch(opType){
        case OperationType::AllSupportedUbuntuRelases:
            // Not required
            retVal = doOperationAllSupportedUbuntuReleases();
            break;
        case OperationType::CurrentUbuntuLTSVersion:
            // Not required
            retVal = doOperationCurrentUbuntuLTSVersion();
            break;
        case OperationType::FetchSha256:
            {
            // Check if given ubuntu release name is not empty
            auto releaseTitle = args[release_title_key].as<std::string>();
            auto releaseCodename = args[release_codename_key].as<std::string>();
            auto version = args[version_key].as<std::string>();

            retVal = doOperationFetchSha256(releaseTitle, releaseCodename, version);

            break;
            }
        case OperationType::ListVersions:
            {
            // Check if given ubuntu release name is not empty
            auto releaseTitle = args[release_title_key].as<std::string>();
            auto releaseCodename = args[release_codename_key].as<std::string>();

            if(releaseTitle.empty() && releaseCodename.empty()){
                std::cout << "Please specify a non empty release title or release codename.";
                return 1;
            }

            retVal = doOperationFindVersions(releaseTitle, releaseCodename);
            break;
            }
    }

    return retVal;
}

std::size_t curlWriteCallback(
            const char* in,
            std::size_t size,
            std::size_t num,
            std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }

bool UCIIParser::obtainJsonFile(){
    // Define URL
    const std::string url = "https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json";

    CURL* curl = curl_easy_init();

    // Set remote URL.
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    /* ask libcurl to show us the verbose output */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Don't wait forever, time out after 10 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Response information.
    int httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (httpCode == 200)
    {
        Json::Reader jsonReader;

        if (jsonReader.parse(*httpData, jsonData))
        {
            // return with no error
            return true;
        }
        else
        {
            std::cout << "Could not parse HTTP data as JSON" << std::endl;
            std::cout << "HTTP data was:\n" << *httpData.get() << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "Couldn't GET from " << url << " - exiting" << std::endl;
        return false;
    }

    return false;
}

int UCIIParser::doOperationAllSupportedUbuntuReleases(){
    bool curlParseOk = obtainJsonFile();

    if(!curlParseOk){
        return 1;
    }

    std::string t_releaseTitle;
    std::string t_releaseCodename;

    for(const Json::Value& product : jsonData["products"]){
        if(product["arch"].asString() != "amd64"){
            continue;
        }
        else{
            t_releaseTitle = product["release_title"].asString();
            t_releaseCodename = product["release_codename"].asString();

            std::cout << t_releaseTitle + " " + t_releaseCodename + " amd64" << std::endl;
        }
    }

    return 0;
}

int UCIIParser::doOperationCurrentUbuntuLTSVersion(){
    bool curlParseOk = obtainJsonFile();

    if(!curlParseOk){
        return 1;
    }

    size_t productCount = jsonData["products"].size();

    Json::Value::const_iterator productToSearch = jsonData["products"].end();
    Json::Value::const_iterator firstProduct = jsonData["products"].begin();

    do{
        productToSearch--;
        if((*productToSearch)["arch"].asString() != "amd64"){
            continue;
        }
        std::string t_releaseTitle = (*productToSearch)["release_title"].asString();
        std::string t_releaseCodename = (*productToSearch)["release_codename"].asString();

        if(t_releaseTitle.find(LTS_keyword) != std::string::npos){
            std::cout << t_releaseTitle + " " + t_releaseCodename + " amd64" << std::endl;
            break;
        }

    }while(productToSearch != firstProduct);

    return 0;
}

int UCIIParser::doOperationFetchSha256(std::string releaseTitle, std::string releaseCodename, std::string version){
    bool curlParseOk = obtainJsonFile();

    if(!curlParseOk){
        return 1;
    }

    return 0;
}

int UCIIParser::doOperationFindVersions(std::string releaseTitle, std::string releaseCodename){
    bool curlParseOk = obtainJsonFile();

    if(!curlParseOk){
        return 1;
    }

    bool searchByReleaseTitle = true;

    // release title is prioritized
    if(releaseTitle.empty()){
        // switch to search by release codename
        searchByReleaseTitle = false;
        std::cout << "Searching by codename: " << releaseCodename << std::endl;
    }
    else{
        std::cout << "Searching by title: " << releaseTitle << std::endl;
    }

    bool found = false;

    for(const Json::Value& product : jsonData["products"]){
        if(product["arch"].asString() != "amd64"){
            continue;
        }
        else{
            std::string t_releaseTitle = product["release_title"].asString();
            std::string t_releaseCodename = product["release_codename"].asString();
            std::string t_availableVersions{""};

            if(searchByReleaseTitle){
                // If product matches
                if(t_releaseTitle == releaseTitle){
                    auto versionNames = product["versions"].getMemberNames();

                    for(auto version: versionNames){
                        t_availableVersions += version + ", ";
                    }

                    t_availableVersions = t_availableVersions.substr(0, t_availableVersions.size() - 2);

                    std::cout << "Please use one of the following version numbers : " << t_availableVersions << std::endl;

                    found = true;

                    break;
                }
            }
            else{
                // If product matches
                if(t_releaseCodename == releaseCodename){
                    auto versionNames = product["versions"].getMemberNames();

                    for(auto version: versionNames){
                        t_availableVersions += version + ", ";
                    }

                    t_availableVersions = t_availableVersions.substr(0, t_availableVersions.size() - 2);

                    std::cout << "Please use one of the following version numbers : " << t_availableVersions << std::endl;

                    found = true;

                    break;
                }
            }
        }
    }

    if(!found){
        if(searchByReleaseTitle){
            std::cout << "Could not find a matching ubuntu release title." << std::endl;
        }
        else{
            std::cout << "Could not find a matching ubuntu release codename." << std::endl;
        }
    }

    return 0;
}
