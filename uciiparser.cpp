#include "uciiparser.h"

#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string>

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
            return 0;
        }
        else
        {
            std::cout << "Could not parse HTTP data as JSON" << std::endl;
            std::cout << "HTTP data was:\n" << *httpData.get() << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Couldn't GET from " << url << " - exiting" << std::endl;
        return 1;
    }

    return 1;
}

int UCIIParser::doOperationAllSupportedUbuntuReleases(){
    obtainJsonFile();

    for(const Json::Value& product : jsonData["products"]){
        if(product["arch"].asString() != "amd64"){
            continue;
        }
        else{
            std::string t_releaseTitle = product["release_title"].asString();
            std::string t_releaseCodename = product["release_codename"].asString();
            std::cout << t_releaseTitle + " " + t_releaseCodename + " amd64" << std::endl;
        }
    }

    return 0;
}

int UCIIParser::doOperationCurrentUbuntuLTSVersion(){
    obtainJsonFile();

    return 0;
}

int UCIIParser::doOperationFetchSha256(std::string releaseTitle, std::string releaseCodename, std::string version){
    obtainJsonFile();

    return 0;
}

int UCIIParser::doOperationFindVersions(std::string releaseTitle, std::string releaseCodename){
    obtainJsonFile();

    return 0;
}
