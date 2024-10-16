/**
 * @file uciiparser.cpp
 * @brief This source file contains the definitions of the ubuntu cloud image information (ucii) derived class and its methods
 * @author Batuhan KOÇ
 * @date 2024-10-16
 */

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
    // Return value of the method
    int retVal = 0;

    // Cast the operation type
    OperationType opType = boost::any_cast<OperationType>(operationType);

    // Perform preliminary checks before starting the given operation
    switch(opType){
        // Print all available amd64 arch. ubuntu releases on the json file
        case OperationType::AllSupportedUbuntuRelases:
            // No preliminary control required

            // Perform related operation
            retVal = doOperationAllSupportedUbuntuReleases();
            break;
        // Print last released ubuntu amd64 arch. lts version name
        case OperationType::CurrentUbuntuLTSVersion:
            // No preliminary control required

            // Perform related operation
            retVal = doOperationCurrentUbuntuLTSVersion();
            break;
        // Fetch sha256 value of the disk1.img item of the given ubuntu release with specific version
        case OperationType::FetchSha256:
            {
            // Fetch and cast arguments related to given operation
            auto releaseTitle = args[release_title_key].as<std::string>();
            auto releaseCodename = args[release_codename_key].as<std::string>();
            auto version = args[version_key].as<std::string>();

            // Double control whether title and codename arguments both empty
            if(releaseTitle.empty() && releaseCodename.empty()){
                std::cout << "Please specify the release tile or release codename of the ubuntu release." << std::endl;
                retVal = 1;
            }
            else{
                // Perform related operation
                retVal = doOperationFetchSha256(releaseTitle, releaseCodename, version);
            }

            break;
            }
        // List the available versions of an ubuntu release by it's title or codename
        case OperationType::ListVersions:
            {
            // Check if given ubuntu release name is not empty
            auto releaseTitle = args[release_title_key].as<std::string>();
            auto releaseCodename = args[release_codename_key].as<std::string>();

            if(releaseTitle.empty() && releaseCodename.empty()){
                std::cout << "Please specify a non empty release title or release codename.";
                retVal = 1;
            }
            else{
                // Perform related operation
                retVal = doOperationFindVersions(releaseTitle, releaseCodename);
            }

            break;
            }
    }

    return retVal;
}

// Curl callback function
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

    // Initialize curl instance
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

    // If returned with no error
    if (httpCode == 200)
    {
        Json::Reader jsonReader;

        // If returned data can be parsed without any error
        if (jsonReader.parse(*httpData, jsonData))
        {
            // return with no error
            return true;
        }
        // If an error occured during json data parsing
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

    // Return immediately if json file reading caused an error
    if(!curlParseOk){
        return 1;
    }

    std::string t_releaseTitle;
    std::string t_releaseCodename;

    // Iterate all elements under products tag
    for(const Json::Value& product : jsonData["products"]){
        // Check if the architecture of the current product is amd64. Continue otherwise.
        if(product["arch"].asString() != "amd64"){
            continue;
        }
        else{
            // If the current release is in amd64 architecture parse the release title and the codename of the given release/product
            t_releaseTitle = product["release_title"].asString();
            t_releaseCodename = product["release_codename"].asString();

            // Print the release title and the codename
            std::cout << t_releaseTitle + " " + t_releaseCodename + " amd64" << std::endl;
        }
    }

    return 0;
}

int UCIIParser::doOperationCurrentUbuntuLTSVersion(){
    bool curlParseOk = obtainJsonFile();

    // Return immediately if json file reading caused an error
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

    // Return immediately if json file reading caused an error
    if(!curlParseOk){
        return 1;
    }

    bool searchByReleaseTitle = true;

    // Search by 'release title' is prioritized
    if(releaseTitle.empty()){
        // Switch to search by release codename
        searchByReleaseTitle = false;
        std::cout << "Searching by codename: " << releaseCodename << std::endl;
    }
    else{
        std::cout << "Searching by title: " << releaseTitle << std::endl;
    }

    // Flag to indicate that a suitable product found by given title or codename
    bool foundProduct = false;
    // Flag to indicate that both a suitable product and a matching version number are found
    bool foundVersion = false;
    // sha256 text value to be stored
    std::string sha256{""};

    // Search among all product values in json file
    for(const Json::Value& product : jsonData["products"]){
        // Continue if the current product's architecture is not amd64
        if(product["arch"].asString() != "amd64"){
            continue;
        }
        else{
            // Fetch release title and codename
            std::string t_releaseTitle = product["release_title"].asString();
            std::string t_releaseCodename = product["release_codename"].asString();
            std::string t_availableVersions{""};

            // If current search is based on release title
            if(searchByReleaseTitle){
                // If product release title is matched
                if(t_releaseTitle == releaseTitle){
                    // Set flag positive
                    foundProduct = true;

                    // Get all version numbers of the current product
                    auto versionNames = product["versions"].getMemberNames();

                    // If the given version number is not included by the available versions
                    if(std::find(versionNames.begin(), versionNames.end(), version) == versionNames.end()){
                        // version not found
                        break;
                    }
                    // If the given version number is included in the available versions list
                    else{
                        // Set flag positive
                        foundVersion = true;
                        // Fetch the sha256 value from the file
                        sha256 = product["versions"][version]["items"]["disk1.img"]["sha256"].asString();
                    }

                    break;
                }
            }
            // If current search is based on release codename
            else{
                // If product matches
                if(t_releaseCodename == releaseCodename){
                    // Set flag positive
                    foundProduct = true;

                    // Get all version numbers of the current product
                    auto versionNames = product["versions"].getMemberNames();

                    // If the given version number is not included by the available versions
                    if(std::find(versionNames.begin(), versionNames.end(), version) == versionNames.end()){
                        // version not found
                        break;
                    }
                    // If the given version number is included in the available versions list
                    else{
                        // Set flag positive
                        foundVersion = true;
                        // Fetch the sha256 value from the file
                        sha256 = product["versions"][version]["items"]["disk1.img"]["sha256"].asString();
                    }

                    break;
                }
            }
        }
    }

    // If product found by title or codename
    if(foundProduct){
        // If both product found and a suitable version is found
        if(foundVersion){
            // Display the sha256 result by given release title
            if(searchByReleaseTitle){
                std::cout << "sha256 of the disk1.img of the ubuntu release (" << releaseTitle << " amd64) is : " << sha256 << std::endl;
            }
            // Display the sha256 result by given release codename
            else{
                std::cout << "sha256 of the disk1.img of the ubuntu release (" << releaseTitle << " amd64) is : " << sha256 << std::endl;
            }
        }
        // If product found but there is no matching version number for this product
        else{
            // Inform user about that there is no matching version and display available version numbers
            if(searchByReleaseTitle){
                std::cout << "Matching ubuntu release found by title but no matching version found." << std::endl;
                doOperationFindVersions(releaseTitle, releaseCodename, true);
            }
            else{
                std::cout << "Matching ubuntu release found by codename but no matching version found." << std::endl;
                doOperationFindVersions(releaseTitle, releaseCodename, true);
            }
        }
    }
    // If there is no product based on the given release title nor release codename
    else{
        // Inform user about that there is no product found
        if(searchByReleaseTitle){
            std::cout << "Could not find a matching ubuntu release title." << std::endl;
        }
        else{
            std::cout << "Could not find a matching ubuntu release codename." << std::endl;
        }
    }

    return 0;
}

int UCIIParser::doOperationFindVersions(std::string releaseTitle, std::string releaseCodename, bool bypassHeadingText){
    bool curlParseOk = obtainJsonFile();

    // Return immediately if json file reading caused an error
    if(!curlParseOk){
        return 1;
    }

    // Default behaviour is searching a product by input release title
    bool searchByReleaseTitle = true;

    // release title is prioritized
    if(releaseTitle.empty()){
        // switch to search by release codename
        searchByReleaseTitle = false;
        // Do not display the information if function is called by doOperationFetchSha256
        if(!bypassHeadingText)
            std::cout << "Searching by codename: " << releaseCodename << std::endl;
    }
    else{
        // Do not display the information if function is called by doOperationFetchSha256
        if(!bypassHeadingText)
            std::cout << "Searching by title: " << releaseTitle << std::endl;
    }

    // Flag to indicate that a product found by given title or codename
    bool found = false;

    // Search among all products in the json file
    for(const Json::Value& product : jsonData["products"]){
        // Continue if the architecture is not amd64
        if(product["arch"].asString() != "amd64"){
            continue;
        }
        // If the architecture is amd64
        else{
            // Fetch release title and the codename
            std::string t_releaseTitle = product["release_title"].asString();
            std::string t_releaseCodename = product["release_codename"].asString();
            std::string t_availableVersions{""};

            // If search is based on the release title
            if(searchByReleaseTitle){
                // If product release title matches
                if(t_releaseTitle == releaseTitle){
                    // Get all available version numbers
                    auto versionNames = product["versions"].getMemberNames();

                    // Merge version numbers together to display
                    for(auto version: versionNames){
                        t_availableVersions += version + ", ";
                    }

                    // Delete last ', ' characters
                    t_availableVersions = t_availableVersions.substr(0, t_availableVersions.size() - 2);

                    // Display the available version numbers for given release title/codename
                    std::cout << "Please use one of the following version numbers : " << t_availableVersions << std::endl;

                    // Set flag as positive
                    found = true;

                    break;
                }
            }
            // If search is based on the release codename
            else{
                // If product codename matches
                if(t_releaseCodename == releaseCodename){
                    // Get all available version numbers
                    auto versionNames = product["versions"].getMemberNames();

                    // Merge version numbers together to display
                    for(auto version: versionNames){
                        t_availableVersions += version + ", ";
                    }

                    // Delete last ', ' characters
                    t_availableVersions = t_availableVersions.substr(0, t_availableVersions.size() - 2);

                    // Display the available version numbers for given release title/codename
                    std::cout << "Please use one of the following version numbers : " << t_availableVersions << std::endl;

                    // Set flag as positive
                    found = true;

                    break;
                }
            }
        }
    }

    // If a product by given release title or codename is not found
    if(!found){
        // Inform user
        if(searchByReleaseTitle){
            std::cout << "Could not find a matching ubuntu release title." << std::endl;
        }
        else{
            std::cout << "Could not find a matching ubuntu release codename." << std::endl;
        }
    }

    return 0;
}
