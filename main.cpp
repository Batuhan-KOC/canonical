/**
 * @file main.cpp
 * @brief This source file contains the main method of the application to create a cli interface over the ubuntu cloud image information parser
 * @author Batuhan KOÇ
 * @date 2024-10-16
 */

#include "uciiparser.h"

#include <iostream>

/**
 *
 * This command line interface wrapper is about performing the following operations:
 * 1. Return a list of all currently supported Ubuntu releases.
 * 2. Return the current Ubuntu LTS version.
 * 3. Return the sha256 of the disk1.img item of a given Ubuntu release.
 * 
 * There are four main cli options available which are:
 * 1. 'help' or 'h'         : To display the application options and their descriptions.
 * 2. 'listall' or 'la'     : To return a list of all currently supported Ubuntu releases.
 * 3. 'listcurr' or 'lc'    : To return the current Ubuntu LTS version.
 * 4. 'sha'                 : To return the sha256 of the disk1.img item of a given ubuntu release
 * 
 * 'help', 'listall' and 'listcurr' options does not require any additional arguments and options. On the other hand,
 * 'sha' required additional informations.
 * 
 * CLI EXAMPLES :
 * 1. Display options
 * To display help menu and options please use the following call methods via cli
 *      ./UCII.exe --help
 *      ./UCII.exe -h
 * 
 * 2. List all ubuntu releases
 * To display all amd64 arch ubuntu releases on the given json file please use the following call methods via cli
 *      ./UCII.exe --listall
 *      ./UCII.exe --la
 * 
 * 3. List current LTS ubuntu release
 * To display the latest LTS amd64 ubuntu release, please use the following call methods via cli
 *      ./UCII.exe --listcurr
 *      ./UCII.exe --lc
 * 
 * 4. Obtain the available version numbers of a given ubuntu release
 * To find all available version numbers of a ubuntu release by its title or codename please use the following call methods via cli
 *      ./UCII.exe --sha --release_title='18.04 LTS'
 *      ./UCII.exe --sha --release_codename='Bionic Beaver'
 * 
 * By using the commands above, user will be able to see the all available version numbers of the 18.04 LTS Bionic Beaver amd64 release of ubuntu
 * 
 * To find the sha256 of the disk1.img item of a given ubuntu release and version number, please use the following call method via cli
 *      ./UCII.exe --sha --release_title='18.04 LTS' --version=20180724
 *      ./UCII.exe --sha --release_codename='Bionic Beaver' --version=20180724
 * 
 * By using the commands above, user will be able to see the same sha256 value of '6d663a8fd5eddd916f4aef4fd06d0f7f4cf0bb191f170b8c84cd2adf297bc5c3'
 * 
 * Note1 : It is usefull to use " ./UCII.exe --sha --release_title='18.04 LTS' " syntax first to list all version numbers first before fetching the 
 * sha256 value
 * 
 * Note2 : release_title and release_codename options can be used at the same time but release title will be used as first option to search.
 */
int main(int argc, const char* argv[]){
    try{
        // Data to store cli input operation type
        UCIIParser::OperationType operationType{UCIIParser::OperationType::Undefined};

        boost::program_options::options_description descriptions("Options");

        // Define cli options
        descriptions.add_options()
            ("help,h", "print usage message")
            ("listall,la", "return a list of all currently supported ubuntu releases")
            ("listcurr,lc", "return the current ubuntu lts version")
            (sha_key, "return the sha256 of the disk1.img item of a given ubuntu release")
            (release_title_key, boost::program_options::value<std::string>()->default_value(""), "Release title of the ubuntu version. (e.g. '14.10', '18.04', '24.04 LTS' etc.)")
            (release_codename_key, boost::program_options::value<std::string>()->default_value(""), "Release codename of the ubuntu version. (e.g. 'Focal Fossa', 'Impish Indri', 'Noble Numbat')")
            (version_key, boost::program_options::value<std::string>()->default_value(""), "Release version of the ubuntu version");

        boost::program_options::variables_map variableMap;

        // Fetch cli options
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, descriptions), variableMap);

        // Instantiate the parser
        UCIIParser ucii;

        // Check cli inputs
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
            if(!variableMap[release_title_key].as<std::string>().empty() || !variableMap[release_codename_key].as<std::string>().empty()){
                if(!variableMap[version_key].as<std::string>().empty()){
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
