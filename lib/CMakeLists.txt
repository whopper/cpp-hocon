# TODO:
#  include(leatherman)
#  get_commit_string(PROJECT_COMMIT)

# Generate a file containing the above version numbers.
configure_file("version.h.in"   "${CMAKE_CURRENT_LIST_DIR}/inc/version.h")

# Set C++ compile flags based on project settings.
set(CMAKE_CXX_FLAGS ${${PROJECT_NAME_UPPER}_CXX_FLAGS})

# Set prefix for logging messages.
leatherman_logging_namespace("puppetlabs.${PROJECT_NAME}")

# Setup compiling the library.
include_directories(inc ${Boost_INCLUDE_DIRS} ${LEATHERMAN_INCLUDE_DIRS})

set(PROJECT_SOURCES
    src/values/abstract_config_value.cc
    src/values/config_boolean.cc
    src/values/config_number.cc
    src/values/config_int.cc
    src/values/config_double.cc
    src/simple_config_origin.cc
)

## An object target is generated that can be used by both the library and test executable targets.
## Without the intermediate target, unexported symbols can't be tested.
add_library(libprojectsrc OBJECT ${PROJECT_SOURCES})
set_target_properties(libprojectsrc PROPERTIES POSITION_INDEPENDENT_CODE true)

add_library(lib${PROJECT_NAME} SHARED $<TARGET_OBJECTS:libprojectsrc>)
set_target_properties(lib${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".so" IMPORT_PREFIX "" IMPORT_SUFFIX ".so.a" VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
target_link_libraries(lib${PROJECT_NAME} PRIVATE
    ${LEATHERMAN_LIBRARIES}
    ${Boost_LIBRARIES}
)

# Generate the export header for restricting symbols exported from the library.
# Restricting symbols has several advantages, noted at https://gcc.gnu.org/wiki/Visibility.
include(GenerateExportHeader)
generate_export_header(lib${PROJECT_NAME} EXPORT_FILE_NAME "${CMAKE_CURRENT_LIST_DIR}/inc/export.h")
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_compiler_export_flags()
elseif (WIN32)
    add_compiler_export_flags()
    add_definitions("-Dlib${PROJECT_NAME_LOWER}_EXPORTS")
endif()

get_filename_component(LIBPATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} NAME)
install(TARGETS lib${PROJECT_NAME} DESTINATION ${LIBPATH})
install(DIRECTORY inc/${PROJECT_NAME} DESTINATION include)

add_subdirectory(tests)
