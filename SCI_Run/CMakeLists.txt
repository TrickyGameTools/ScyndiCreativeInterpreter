cmake_minimum_required(VERSION 3.16)

project(SCI_Run LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Lua: Verzamel alle .c bestanden in de source directory
file(GLOB LUA_SOURCES CONFIGURE_DEPENDS "../../../Libs/Lunatic/Lua/Raw/src/*.c")
# Lua: Verwijder de executable bronbestanden
list(FILTER LUA_SOURCES EXCLUDE REGEX "/(lua|luac|onelua)\\.c$")


add_executable(SCI_Run
    ${LUA_SOURCES}
    SCI_Run.cpp
    SCI_Audio.cpp
    SCI_Audio.hpp
    SCI_Config.cpp
    SCI_Config.hpp
    SCI_Crash.cpp
    SCI_Crash.hpp
    SCI_Graphics.cpp
    SCI_Graphics.hpp
    SCI_JCR.cpp
    SCI_JCR.hpp
    SCI_Kthura.cpp
    SCI_Kthura.hpp
    SCI_Load.cpp
    SCI_Load.hpp
    SCI_Run.cpp
    SCI_Script.cpp
    API/API_Audio.cpp
    API/API_Bank.cpp
    API/API_Bank.hpp
    API/API_BlopPlasma.cpp
    API/API_DebugConsole.cpp
    API/API_DebugConsole.hpp
    API/API_Events.cpp
    API/API_GINIE.cpp
    API/API_Graphics.cpp
    API/API_InterState.cpp
    API/API_JCR.cpp
    API/API_Kthura.cpp
    API/API_Medals.cpp
    API/API_Random.cpp
    API/API_RealFile.cpp
    API/API_SGFile.cpp
    API/API_SGJCR.cpp
    API/API_SGJCR_Enum.hpp
    API/API_Statistician.cpp
    API/API_String.cpp
    API/API_Vars.cpp
    API/SCI_InstallAPIs.hpp
    ../SCI_Share/SCI_GlobalConfig.cpp
    ../SCI_Share/SCI_GlobalConfig.hpp
    ../SCI_Share/SCI_Header.hpp
    ../SCI_Share/Version.hpp
    ../../../Libs/Statistician/Statistician.cpp
    ../../../Libs/Statistician/Statistician.hpp
    ../../../Libs/Statistician/Statistician_LoadJCR6.cpp
    ../../../Libs/Statistician/Statistician_LoadJCR6.hpp
    ../../../Libs/Statistician/Statistician_SaveJCR6.cpp
    ../../../Libs/Medals/Medals.cpp
    ../../../Libs/Medals/Medals.hpp
    ../../../Libs/JCR6/Source/JCR6_Core.cpp
    ../../../Libs/JCR6/Source/JCR6_JQL.cpp
    ../../../Libs/JCR6/Source/JCR6_QuakePAK.cpp
    ../../../Libs/JCR6/Source/JCR6_RealDir.cpp
    ../../../Libs/JCR6/Source/JCR6_WAD.cpp
    ../../../Libs/JCR6/Source/JCR6_Write.cpp
    ../../../Libs/JCR6/Source/JCR6_zlib-ng.cpp
    ../../../Libs/Units/Source/SlyvArgParse.cpp
    ../../../Libs/Units/Source/SlyvAsk.cpp
    ../../../Libs/Units/Source/SlyvBank.cpp
    ../../../Libs/Units/Source/SlyvDirry.cpp
    ../../../Libs/Units/Source/SlyvMD5.cpp
    ../../../Libs/Units/Source/SlyvQCol.cpp
    ../../../Libs/Units/Source/SlyvSTOI.cpp
    ../../../Libs/Units/Source/SlyvString.cpp
    ../../../Libs/Units/Source/SlyvStream.cpp
    ../../../Libs/Units/Source/SlyvTime.cpp
    ../../../Libs/Units/Source/SlyvOpenURL.cpp
    ../../../Libs/Units/Source/SlyvDir.cpp
    ../../../Libs/Units/Source/SlyvBank.cpp
    ../../../Libs/Units/Source/SlyvVolumes.cpp
    ../../../Libs/Units/Source/SlyvVecStringSaver.cpp
    ../../../Libs/Units/Source/SlyvDijkstra.cpp
    ../../../Libs/Kthura/Source/Kthura_Compiler.cpp
    ../../../Libs/Kthura/Source/Kthura_Core.cpp
    ../../../Libs/Kthura/Source/Kthura_Draw.cpp
    ../../../Libs/Kthura/Source/Kthura_Draw_TQSG.cpp
    ../../../Libs/Kthura/Source/Kthura_LoadCompiled.cpp
    ../../../Libs/Kthura/Source/Kthura_Save.cpp
    ../../../Libs/Kthura/Source/Kthura_Walk_Dijkstra.cpp
    ../../../Libs/TQSL/Source/TQSA.cpp
    ../../../Libs/TQSL/Source/TQSE.cpp
    ../../../Libs/TQSL/Source/TQSG.cpp
    ../../../Libs/TQSL/Source/TQSG_BlopPlasma.cpp
    ../../../Libs/Lunatic/Lunatic.cpp
    ../../../Libs/Lunatic/Lunatic.hpp
)

if(EXISTS "/run/host/usr/local/include/SDL2")
    set(SDL2_LOCAL_INCLUDE "/run/host/usr/local/include")
    set(SDL2_LOCAL_LIB "/run/host/usr/local/lib")
else()
    set(SDL2_LOCAL_INCLUDE "/usr/local/include")
    set(SDL2_LOCAL_LIB "/usr/local/lib")
endif()



target_include_directories(SCI_Run PRIVATE
    ${SDL2_LOCAL_INCLUDE}
    ../../../Libs/Units/Headers
    ../../../Libs/TQSL/Headers
    ../../../Libs/JCR6/Headers
    ../../../Libs/Kthura/Headers
    #../../../Libs/June19/head
    ../../../Libs/Lunatic
    ../../../Libs/Statistician/
    ../../../Libs/Medals
)


include(GNUInstallDirs)
install(TARGETS SCI_Run
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



target_link_directories(SCI_Run PRIVATE ${SDL2_LOCAL_LIB})
target_link_libraries(SCI_Run PRIVATE
    SDL2
    SDL2_image
    SDL2_mixer
    ${CMAKE_SOURCE_DIR}/../../../Libs/JCR6/3rdParty/zlib-ng/build/libz-ng.a
)


set_target_properties(SCI_Run PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../SCI_Exe
)


