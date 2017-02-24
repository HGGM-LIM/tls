# Author: Ali Uneri
# Date: 2012-10-28

set(EP_OPTION_NAME "USE_${EP_NAME}")
set(EP_REQUIRED_PROJECTS Git)
set(EP_PATCH "${CMAKE_CURRENT_LIST_DIR}/Patches/${EP_NAME}.patch")
set(EP_OPTION_DESCRIPTION "Insight Segmentation and Registration Toolkit")

cma_list(APPEND EP_REQUIRED_PROJECTS CUDA IF USE_CUDA)
cma_list(APPEND EP_REQUIRED_PROJECTS DCMTK IF USE_DCMTK)
cma_list(APPEND EP_REQUIRED_PROJECTS GDCM IF USE_GDCM)
cma_list(APPEND EP_REQUIRED_PROJECTS OpenCV IF USE_OpenCV)
cma_list(APPEND EP_REQUIRED_PROJECTS VTK IF ${EP_OPTION_NAME}_VtkGlue)
cma_list(APPEND EP_REQUIRED_PROJECTS zlib IF USE_zlib)

if(USE_Slicer)
  set(EP_URL "git://github.com/Slicer/ITK.git")
elseif(USE_MITK)
  set(EP_URL "git://itk.org/ITK.git")
else()
  set(EP_URL "git://itk.org/ITK.git")
endif()

cma_envvar(@LIBRARYPATH@ PREPEND "@BINARY_DIR@/@LIBDIR@/@INTDIR@")

cma_end_definition()
# -----------------------------------------------------------------------------

cmake_dependent_option(${EP_OPTION_NAME}_VtkGlue "Enable ITKVtkGlue module" OFF ${EP_OPTION_NAME} OFF)
cmake_dependent_option(ITKV3_COMPATIBILITY "Enable ITKV3_COMPATIBILITY module" ON ${EP_OPTION_NAME} ON)

if(WIN32)
  string(LENGTH "${PROJECT_BINARY_DIR}/${EP_NAME}-build" LENGTH)
  if(LENGTH GREATER 50)
    message(FATAL_ERROR "Shorter path for ${PROJECT_NAME} build directory is required, since ITK path is ${LENGTH} > 50 characters")
  endif()
endif()

set(EP_CMAKE_ARGS
  -DBUILD_EXAMPLES:BOOL=OFF
  -DBUILD_TESTING:BOOL=OFF
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
  -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
  -DITK_USE_SYSTEM_DCMTK:BOOL=${USE_DCMTK}
  -DITK_USE_SYSTEM_GDCM:BOOL=${USE_GDCM}
  -DITK_USE_SYSTEM_ZLIB:BOOL=${USE_zlib}
  -DITKV3_COMPATIBILITY:BOOL=${ITKV3_COMPATIBILITY}
  -DModule_ITKIODCMTK:BOOL=${USE_DCMTK}
  -DModule_ITKReview:BOOL=ON
  -DModule_ITKVideoBridgeOpenCV:BOOL=OFF
  -DModule_ITKVtkGlue:BOOL=${${EP_OPTION_NAME}_VtkGlue})

if(USE_Slicer)
  if(${PROJECT_NAME}_Slicer_VERSION VERSION_EQUAL "4.4")
    set(EP_VERSION "56fae278ad0ae805da4f4dbea5a4b9979cf4262c")
    list(APPEND EP_CMAKE_ARGS -DModule_MGHIO:BOOL=OFF)
  elseif(${PROJECT_NAME}_Slicer_VERSION VERSION_EQUAL "4.5")
    set(EP_VERSION "03055046e2693349937339975df4e16b598609c1")
    list(APPEND EP_CMAKE_ARGS -DModule_MGHIO:BOOL=ON)
  else()
    message(FATAL_ERROR "Slicer version ${${PROJECT_NAME}_Slicer_VERSION} is not supported")
  endif()
  list(APPEND EP_CMAKE_ARGS -DITK_USE_GPU:BOOL=OFF)
elseif(USE_MITK)
  set(EP_VERSION "v4.7.1")
  list(APPEND EP_CMAKE_ARGS -DITK_USE_GPU:BOOL=OFF)
else()
  set(EP_VERSION "v4.9.0")
  list(APPEND EP_CMAKE_ARGS -DITK_USE_GPU:BOOL=${USE_CUDA})
endif()

if(USE_DCMTK)
  cmake_minimum_required(VERSION 3.0)
  list(APPEND EP_CMAKE_ARGS -DDCMTK_DIR:PATH=${${PROJECT_NAME}_DCMTK_DIR})
endif()
if(USE_GDCM)
  list(APPEND EP_CMAKE_ARGS -DGDCM_DIR:PATH=${${PROJECT_NAME}_GDCM_DIR})
endif()
if(USE_MITK OR USE_SimpleITK OR USE_Slicer)
  list(APPEND EP_CMAKE_ARGS -DBUILD_SHARED_LIBS:BOOL=ON)
else()
  list(APPEND EP_CMAKE_ARGS -DBUILD_SHARED_LIBS:BOOL=OFF)
endif()
if(USE_OpenCV)
  list(APPEND EP_CMAKE_ARGS -DOpenCV_DIR:PATH=${${PROJECT_NAME}_OpenCV_DIR})
endif()
if(USE_zlib)
  list(APPEND EP_CMAKE_ARGS
    -DZLIB_ROOT:PATH=${${PROJECT_NAME}_ZLIB_ROOT}
    -DZLIB_INCLUDE_DIR:PATH=${${PROJECT_NAME}_ZLIB_INCLUDE_DIR}
    -DZLIB_LIBRARY:FILEPATH=${${PROJECT_NAME}_ZLIB_LIBRARY})
endif()
if(${EP_OPTION_NAME}_VtkGlue)
  list(APPEND EP_CMAKE_ARGS -DVTK_DIR:PATH=${${PROJECT_NAME}_VTK_DIR})
endif()

ExternalProject_Add(${EP_NAME}
  DEPENDS ${EP_REQUIRED_PROJECTS}
  # download
  GIT_REPOSITORY ${EP_URL}
  GIT_TAG ${EP_VERSION}
  # patch
  # update
  UPDATE_COMMAND ""
  # configure
  SOURCE_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}
  CMAKE_ARGS ${EP_CMAKE_ARGS}
  # build
  BINARY_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}-build
  # install
  INSTALL_COMMAND ""
  # test
  )

set(${PROJECT_NAME}_${EP_NAME}_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}-build" CACHE INTERNAL "")
