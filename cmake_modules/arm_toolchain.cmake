# - Set up arm toolchain
#
#
message (STATUS "by MWO: use arm toolchain.")

set (ONYX_SDK_ROOT      "/opt/onyx/arm")
set (USE_CORTEX_A8      ON)
set (LINK_ZLIB_DEFAULT  ON)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/libs)
set (TEST_OUTPUT_PATH       ${PROJECT_BINARY_DIR}/unittests)
set (QT_PLUGINS_OUTPUT_PATH ${PROJECT_BINARY_DIR}/plugins)
set (ONYX_MWO_ROOT      "/opt/onyx/mwo")

# Define the dependency libraries root path.
if (ONYX_SDK_ROOT)
    message (STATUS "by MWO: ONYX_SDK_ROOT is set to ${ONYX_SDK_ROOT}")
    set     (CMAKE_FIND_ROOT_PATH ${ONYX_SDK_ROOT})
    link_directories(${ONYX_SDK_ROOT}/lib ${ONYX_MWO_ROOT}/lib)
else (ONYX_SDK_ROOT)
    message (STATUS "by MWO: ONYX_SDK_ROOT is not set. Use default: /opt/onyx/arm/")
    set     (CMAKE_FIND_ROOT_PATH "/opt/onyx/arm/")
    link_directories("/opt/onyx/arm/lib")
endif (ONYX_SDK_ROOT)
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

if (${CMAKE_FIND_ROOT_PATH})
    set     (CMAKE_INSTALL_PREFIX  ${CMAKE_FIND_ROOT_PATH})
else (${CMAKE_FIND_ROOT_PATH})
    set     (CMAKE_INSTALL_PREFIX  "/opt/onyx/arm/")
endif (${CMAKE_FIND_ROOT_PATH})

# For libraries and headers in the target directories
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Set up development build mode
set (CMAKE_CXX_FLAGS "-s ")
if (USE_CORTEX_A8)
    message (STATUS "by MWO: USE_CORTEX_A8 is set.")
    set     (CMAKE_CXX_FLAGS "-s -fPIC -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp ")
endif (USE_CORTEX_A8)

set (CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Devel."
    FORCE)

set (PDF_LIB libfpdfemb_arm.a)
set (TTS_LIB AiSound4)

if (LINK_ZLIB_DEFAULT)
    set     (ADD_LIB m rt pthread dl z directfb fusion direct)
else (LINK_ZLIB_DEFAULT)
    set     (ADD_LIB m rt pthread dl)
endif (LINK_ZLIB_DEFAULT)

add_definitions(-DBUILD_FOR_ARM)
add_definitions(-DENABLE_EINK_SCREEN)
add_definitions(-DBS60_INIT_MAIN)
add_definitions(-DBS80_INIT_MAIN)
add_definitions(-DBS97_INIT_MAIN)
add_definitions(-DSFM_M25P20) 

add_definitions(-DSHARE_ROOT="/usr/share")

add_definitions(-DDATADIR="./share/directfb-examples")
add_definitions(-DFONT="./share/font/decker.ttf")
