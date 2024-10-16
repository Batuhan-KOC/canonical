# canonical
Canonical technical assessment

## Used thirdparty libraries:
+ Curl
+ Jsoncpp
+ Boost

## Preperation on ubuntu
On ubuntu, please install the required thirdparty modules by command lines below:
apt-get install libcurl4-openssl-dev
apt-get install libjsoncpp-dev
apt-get install libboost-all-dev
(optional, if required) apt-get install cmake

## Preperation on windows
On windows, please follow the instructions below:
1. Download and install the msys2 from the [link](https://www.msys2.org/)
2. Open the msys2.exe on the installation folder and use the command :
   
     pacman -S mingw-w64-ucrt-x86_64-gcc
5. Open the msys2.exe on the installation folder and use the following commands:
   
     pacman -S mingw-w64-x86_64-curl
   
     pacman -S mingw-w64-x86_64-boost
   
     pacman -S mingw-w64-x86_64-jsoncpp
   
     (optional, if required) pacman -S mingw-w64-x86_64-cmake
   
     (optional, if required) pacman -S mingw-w64-x86_64-cmake-gui
   
5. Use cmake or cmake-gui to build the project.
6. Select the mingw64 as compiler.
7. A classic cmake configure and build process is used.

## Usage
This command line interface wrapper is about performing the following operations:
1. Return a list of all currently supported Ubuntu releases.
2. Return the current Ubuntu LTS version.
3. Return the sha256 of the disk1.img item of a given Ubuntu release.

There are four main cli options available which are:
1. 'help' or 'h'         : To display the application options and their descriptions.
2. 'listall' or 'la'     : To return a list of all currently supported Ubuntu releases.
3. 'listcurr' or 'lc'    : To return the current Ubuntu LTS version.
4. 'sha'                 : To return the sha256 of the disk1.img item of a given ubuntu release

'help', 'listall' and 'listcurr' options does not require any additional arguments and options. On the other hand,
'sha' required additional informations.

CLI EXAMPLES :
1. Display options
To display help menu and options please use the following call methods via cli

     ./UCII.exe --help
   
     ./UCII.exe -h

3. List all ubuntu releases
To display all amd64 arch ubuntu releases on the given json file please use the following call methods via cli

     ./UCII.exe --listall
   
     ./UCII.exe --la

5. List current LTS ubuntu release
To display the latest LTS amd64 ubuntu release, please use the following call methods via cli

     ./UCII.exe --listcurr
   
     ./UCII.exe --lc

7. Obtain the available version numbers of a given ubuntu release
To find all available version numbers of a ubuntu release by its title or codename please use the following call methods via cli

     ./UCII.exe --sha --release_title='18.04 LTS'
   
     ./UCII.exe --sha --release_codename='Bionic Beaver'

By using the commands above, user will be able to see the all available version numbers of the 18.04 LTS Bionic Beaver amd64 release of ubuntu

To find the sha256 of the disk1.img item of a given ubuntu release and version number, please use the following call method via cli

     ./UCII.exe --sha --release_title='18.04 LTS' --version=20180724
     
     ./UCII.exe --sha --release_codename='Bionic Beaver' --version=20180724

By using the commands above, user will be able to see the same sha256 value of '6d663a8fd5eddd916f4aef4fd06d0f7f4cf0bb191f170b8c84cd2adf297bc5c3'

Note1 : It is usefull to use " ./UCII.exe --sha --release_title='18.04 LTS' " syntax first to list all version numbers first before fetching the 
sha256 value

Note2 : release_title and release_codename options can be used at the same time but release title will be used as first option to search.

