#include "uciiparser.h"

#include <curl/curl.h>
#include <json/json.h>
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

    OperationType opType = boost::any_cast<OperationType>(operationType);
    switch(opType){
        case OperationType::AllSupportedUbuntuRelases:
            // Not required
            doOperationAllSupportedUbuntuReleases();
            break;
        case OperationType::CurrentUbuntuLTSVersion:
            // Not required
            doOperationCurrentUbuntuLTSVersion();
            break;
        case OperationType::FetchSha256:
            // Check if given ubuntu release name is not empty
            auto ubuntuReleaseName = args[sha_key].as<std::string>();

            if(ubuntuReleaseName.empty()){
                std::cout << "Please type a non empty release name" << std::endl;
                return 1;
            }

            doOperationFetchSha256(ubuntuReleaseName);

            break;
    }

    
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
        std::cout << "\nGot successful response from " << url << std::endl;

        // Response looks good - done using Curl now.  Try to parse the results
        // and print them out.
        Json::Value jsonData;
        Json::Reader jsonReader;

        if (jsonReader.parse(*httpData, jsonData))
        {
            std::ofstream out("output.json");
            out << *httpData;
            out.close();

            std::cout << "done\n";
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
}

int UCIIParser::doOperationAllSupportedUbuntuReleases(){
    obtainJsonFile();
}

int UCIIParser::doOperationCurrentUbuntuLTSVersion(){
    obtainJsonFile();
}

int UCIIParser::doOperationFetchSha256(std::string releaseName){
    obtainJsonFile();
}
