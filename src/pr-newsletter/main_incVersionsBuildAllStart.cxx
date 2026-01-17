#include "incVersions.H"
#include <filesystem>
#include <iostream>
#include <string>

/// @brief  OBSOLETE - replaced by new concept: see pr-local-scripts/script/version-sub-inc_build-with-sw_start.sh and dependencies
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, const char* argv[])
{
    std::cerr << "\n\n ATTENTION!    main_incVersionsBuildAllStart.cxx replaced by pr-local-scripts/script/version-sub-inc_build-with-sw_start.sh !!!\n\n\n";
    if (true) return 1;


    //////////////////////////////////////////////
    // old see above:

    if (argc < 2) {
        std::cerr << "Aufruf ohne Argument <Projekt>" << std::endl;
        return 1;
    }

    std::string project(argv[1]);
    std::cout << "project: '" << project << "'\n";
    auto folder = std::filesystem::current_path();
    auto error = incVersionsBuildStart(folder, project);
    if (error != "") {
        std::cout << error << "\n";
    } else {
        std::cout << "incVersionsBuildStart erfolgreich!\n\n";
    }
    return 0;
}