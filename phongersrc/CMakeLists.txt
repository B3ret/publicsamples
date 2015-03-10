# hopfer.benjamin@gmail.com
# http://benjaminhopfer.com
# 
# Phonger example application for Qt / vtk
# 32bit/64bit cross compile
#
# CMAKE Build file

# --------------------------------
# SET UP VERSION INFORMATION
cmake_minimum_required(VERSION 3.1)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()
if(POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

project("PhongerExample")

# --------------------------------
# Set variables for 32 or 64 bit
if(${CMAKE_SIZEOF_VOID_P} MATCHES 4)
	message(STATUS "CONFIGURING FOR 32 BIT!")
	set(IS_64_BIT 0)
	set(BIT_3264 "32")
	set(BIT_64_ "")
	set(VS_3264 "Win32")
	set(X86_X64 "x86")
else(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	message(STATUS "CONFIGURING FOR 64 BIT!")
	set(IS_64_BIT 1)
	set(BIT_3264 "64")
	set(BIT_64_ "64_")
	set(VS_3264 "x64")
	set(X86_X64 "x64")
endif()

set(PROJ_NAME "PhongerExample${BIT_3264}")

# --------------------------------
# Set Qt, vtk paths up for 32 or 64 bit
# After this section you shouldn't have to adapt any more paths to your system.
set(QT_CMAKE_PREFIX_PATH "C:/Qt/Qt5.3.1.${BIT_3264}bit/5.3/msvc2013_${BIT_64_}opengl")
set(QT_QMAKE_EXECUTABLE PATH C:/Qt/Qt5.3.1.${BIT_3264}bit/5.3/msvc2013_${BIT_64_}opengl/bin/qmake.exe)
set(VTK_DIR C:/_cpplibs/vtk/bin.qt5.${BIT_3264}bit)
set(VS_DIR "C:/Program Files (x86)/Microsoft Visual Studio 12.0")
set(MS_SDK_DIR "C:/Program Files (x86)/Microsoft SDKs")

# --------------------------------
# Set up vtk
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set(VTK_BIN_DIR_DEBUG ${VTK_RUNTIME_LIBRARY_DIRS}/Debug)
set(VTK_BIN_DIR_RELEASE ${VTK_RUNTIME_LIBRARY_DIRS}/Release)
set(VTK_BIN_DIR_RELWITHDEBINFO ${VTK_RUNTIME_LIBRARY_DIRS}/RelWithDebInfo)

# --------------------------------
# Set up Qt
set(CMAKE_PREFIX_PATH ${QT_CMAKE_PREFIX_PATH})
set(QT_USE_IMPORTED_TARGETS TRUE)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
set(QT_BIN_DIR ${QT_CMAKE_PREFIX_PATH}/bin)

# --------------------------------
# Set up Windows Libraries
if(${IS_64_BIT} MATCHES 1)
	set(WIN_LIB_DIR ${MS_SDK_DIR}"/Windows/v7.1A/Lib/x64")
else()
	set(WIN_LIB_DIR ${MS_SDK_DIR}"/Windows/v7.1A/Lib")
endif()

set(WIN_LIBRARIES ${WIN_LIB_DIR}/IPHlpApi.Lib)

# --------------------------------
# General include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CRYPT_INCLUDE_DIR})

# --------------------------------
# Enable object file parallelism on Visual C++ compilter
if(WIN32 AND MSVC) # Check if we are using the Visual Studio compiler
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# --------------------------------
# The sub files for main project, test project, and installer project
include(MainProject.cmake)