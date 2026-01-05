cmake_minimum_required(VERSION 3.26)

project(app VERSION ${APP_VERSION})

# debug display of all cmake environment variables
#execute_process(
#    COMMAND ${CMAKE_COMMAND} -E environment
#)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /FS /MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FS /MP")
set(CMAKE_DEBUG_POSTFIX D)
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CONAN_PROFILE)
    set(CONAN_PROFILE "Default" CACHE STRING "Choose the conan profile" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
elseif(NOT(CMAKE_BUILD_TYPE STREQUAL "Debug") AND NOT(CMAKE_BUILD_TYPE STREQUAL "Release"))
	message(FATAL_ERROR "Choose the type of build (Debug or Release or RelWithDebInfo only)")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_PLATFORM "64")
	set(CMAKE_GENERATOR_PLATFORM x64)
else()
	set(CMAKE_PLATFORM "32")
	set(CMAKE_GENERATOR_PLATFORM Win32)
endif()

if (NOT CMAKE_BINARY_DIR)
    set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}${CMAKE_PLATFORM}" CACHE STRING "Choose the binary directory" FORCE)
endif()
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/exe/${CMAKE_BUILD_TYPE}${CMAKE_PLATFORM}" CACHE STRING "Choose the runtime output directory" FORCE)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})


message(STATUS "--- Conan profile: ${CONAN_PROFILE}")
message(STATUS "--- Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "--- Building ${CMAKE_BUILD_TYPE} - ${CMAKE_PLATFORM} bits")
message(STATUS "--- Output directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

execute_process(
	COMMAND conan install ${CMAKE_SOURCE_DIR}/conanfile.txt
	--profile:host=${CONAN_PROFILE}
	--profile:build=${CONAN_PROFILE}
	-of ${CMAKE_BINARY_DIR}
	--build=missing
	--deployer=runtime_deploy
	--deployer-folder=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	-vv
	RESULT_VARIABLE conan_install_result
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
 DESTINATION .
 FILES_MATCHING
 PATTERN "*.*")
# 2. Ajouter une cible personnalisée pour exécuter l'installation
add_custom_target(copy_assets_to_runtime ALL
 COMMAND ${CMAKE_COMMAND} --install . --prefix
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
 COMMENT "Installing assets to runtime directory"
)


if(NOT conan_install_result EQUAL 0)
    message(FATAL_ERROR "Conan install failed with error code ${conan_install_result}")
else()
    message(STATUS "Conan install completed successfully")
endif()

set(CONAN_GENERATORS_DIR "${CMAKE_BINARY_DIR}/build/generators")
list(PREPEND CMAKE_PREFIX_PATH "${CONAN_GENERATORS_DIR}")

find_package(GTest REQUIRED)
find_package(SFML 2.6.2 REQUIRED COMPONENTS system window graphics)
enable_testing()

add_subdirectory(src)