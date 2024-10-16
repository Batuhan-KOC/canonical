#include <iostream>

#include "uciiparser.h"

int main(int argc, const char* argv[]){
    try{
        UCIIParser::OperationType operationType{UCIIParser::OperationType::Undefined};

        boost::program_options::options_description descriptions("Options");

        descriptions.add_options()
            ("help,h", "print usage message")
            ("listall,la", "return a list of all currently supported ubuntu releases")
            ("listcurr,lc", "return the current ubuntu lts version")
            ("sha", "return the sha256 of the disk1.img item of a given ubuntu release");

        boost::program_options::variables_map variableMap;

        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, descriptions), variableMap);

        if(variableMap.count("help")){
            std::cout << descriptions << std::endl;
            return 0;
        }
        else if(variableMap.count("listall")){
            // return a list of all currently supported ubuntu releases
        }
        else if(variableMap.count("listcurr")){
            // return the current ubuntu lts version
        }
        else if(variableMap.count("sha")){
            // return the sha256 of the disk1.img item of a given ubuntu release
        }
    }
    catch(std::exception& e){
        std::cerr << e.what() << "\n";
    }

    return 0;
}
