
# Json
if(USE_SYSTEM_JSON)
    find_package(nlohmann_json 3.10.5 REQUIRED)
else()
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    add_subdirectory(nlohmann_json EXCLUDE_FROM_ALL)
endif()

# Units
if(USE_SYSTEM_UNITS)
find_package(units 2.3.1 REQUIRED)
else()
set(BUILD_TESTS OFF CACHE INTERNAL "")
add_subdirectory(units EXCLUDE_FROM_ALL)
add_library(units::units ALIAS units)
endif()

# Parser-SPEF
if(USE_SYSTEM_PARSER_SPEF)
find_package(Parser-SPEF 3.9 REQUIRED)
else()
set(ENABLE_PARSER_SPEF_TESTS OFF CACHE INTERNAL "")
add_subdirectory(Parser-SPEF EXCLUDE_FROM_ALL)
add_library(Parser-SPEF::Parser-SPEF ALIAS Parser-SPEF)
endif()

# Taskflow
if(USE_SYSTEM_TASKFLOW)
find_package(Cpp-Taskflow 2.2.0 REQUIRED)
else()
    set(TF_BUILD_TESTS OFF CACHE INTERNAL "")
    set(TF_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    add_subdirectory(taskflow EXCLUDE_FROM_ALL)
    add_library(Cpp-Taskflow::Cpp-Taskflow ALIAS Cpp-Taskflow)
endif()
