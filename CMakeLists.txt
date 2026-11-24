cmake_minimum_required(VERSION 4.0.3)

project(VkSandbox)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#third parties
# add_subdirectory(ext)

include(cxxflags.cmake)

add_subdirectory(host)
