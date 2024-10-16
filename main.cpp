#include "uciiparser.h"

#include <iostream>

int main(int argc, const char* argv[]){
    try{
        UCIIParser::OperationType operationType{UCIIParser::OperationType::Undefined};

        boost::program_options::options_description descriptions("Options");

        descriptions.add_options()
            ("help,h", "print usage message")
            ("listall,la", "return a list of all currently supported ubuntu releases")
            ("listcurr,lc", "return the current ubuntu lts version")
            (sha_key, "return the sha256 of the disk1.img item of a given ubuntu release")
            (release_title_key, boost::program_options::value<std::string>()->default_value(""), "Release title of the ubuntu version. (e.g. '14.10', '18.04', '24.04 LTS' etc.)")
            (release_codename_key, boost::program_options::value<std::string>()->default_value(""), "Release codename of the ubuntu version. (e.g. 'Focal Fossa', 'Impish Indri', 'Noble Numbat')")
            (version_key, boost::program_options::value<std::string>()->default_value(""), "Release version of the ubuntu version");

        boost::program_options::variables_map variableMap;

        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, descriptions), variableMap);

        UCIIParser ucii;

        if(variableMap.count("help")){
            // return available cli options
            std::cout << descriptions << std::endl;
        }
        else if(variableMap.count("listall")){
            // return a list of all currently supported ubuntu releases
            ucii.requestOperation(UCIIParser::OperationType::AllSupportedUbuntuRelases);
        }
        else if(variableMap.count("listcurr")){
            // return the current ubuntu lts version
            ucii.requestOperation(UCIIParser::OperationType::CurrentUbuntuLTSVersion);
        }
        else if(variableMap.count(sha_key)){
            if(variableMap.count(release_title_key) || variableMap.count(release_codename_key)){
                if(variableMap.count(version_key)){
                    // return the sha256 of the disk1.img item of a given ubuntu release
                    ucii.requestOperation(UCIIParser::OperationType::FetchSha256, variableMap);
                }
                else{
                    // Find available version numbers for the given ubuntu release
                    ucii.requestOperation(UCIIParser::OperationType::ListVersions, variableMap);
                }
            }
            else{
                std::cout << "Please specify the release tile or release codename of the ubuntu release." << std::endl;
            }
        }
        else{
            std::cout << "No command found." << std::endl;
            std::cout << descriptions << std::endl;
        }
    }
    catch(std::exception& e){
        std::cerr << e.what() << "\n";
    }

    return 0;
}
