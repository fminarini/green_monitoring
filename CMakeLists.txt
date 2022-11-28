cmake_minimum_required(VERSION 3.20 FATAL_ERROR) 

#project name
project(KIG VERSION 0.1 DESCRIPTION "Keep IT Green")

# Declare dependence from toml by toruniina
#add_library (
                #toml_lib
                #toml/include/toml.hpp
                #)

# Declare the library target.
add_library(${PROJECT_NAME} SHARED
                source/KIG.cpp
                source/KIG.h
)

#include_directories(${CMAKE_SOURCE_DIR}/toml/include) 
#target_link_libraries(toml_lib ${$PROJECT_NAME})

# Configure the directories to search for header files.
# Set the version property.
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
# Set the shared object version property to the project's major version.
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
# Set the public header property to the one with the actual API.
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER source/KIG.h)

#set_target_properties(toml_lib PROPERTIES LINKER_LANGUAGE CXX)

# For access to standard installation directory variables (CMAKE_INSTALL_xDIR).
include(GNUInstallDirs)
# Set library shared object and API header file to install.
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
