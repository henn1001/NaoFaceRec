cmake_minimum_required(VERSION 2.8)

# Give a name to the project.
project(nfr)

# You need this to find the qiBuild CMake framework
find_package(qibuild)

#Facevacs Headers 
include_directories("./include")
#Facevacs Librarys
link_directories("../../lib")    
#Source Folder
include_directories("./src")

# Add SRC directory
add_subdirectory(src)


