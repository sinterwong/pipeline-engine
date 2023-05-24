FUNCTION(flowengine_app_extract_version)
    FILE(READ "${CMAKE_CURRENT_LIST_DIR}/src/common/version.hpp" file_contents)
    STRING(REGEX MATCH "FLOWENGINE_VER_MAJOR ([0-9]+)" _ "${file_contents}")

    IF(NOT CMAKE_MATCH_COUNT EQUAL 1)
        MESSAGE(FATAL_ERROR "Could not extract major version number from version.hpp")
    ENDIF()

    SET(ver_major ${CMAKE_MATCH_1})

    STRING(REGEX MATCH "FLOWENGINE_VER_MINOR ([0-9]+)" _ "${file_contents}")

    IF(NOT CMAKE_MATCH_COUNT EQUAL 1)
        MESSAGE(FATAL_ERROR "Could not extract minor version number from version.hpp")
    ENDIF()

    SET(ver_minor ${CMAKE_MATCH_1})
    STRING(REGEX MATCH "FLOWENGINE_VER_PATCH ([0-9]+)" _ "${file_contents}")

    IF(NOT CMAKE_MATCH_COUNT EQUAL 1)
        MESSAGE(FATAL_ERROR "Could not extract patch version number from version.hpp")
    ENDIF()

    SET(ver_patch ${CMAKE_MATCH_1})

    SET(FLOWENGINE_APP_VERSION_MAJOR ${ver_major} PARENT_SCOPE)
    SET(FLOWENGINE_APP_VERSION "${ver_major}.${ver_minor}.${ver_patch}" PARENT_SCOPE)
ENDFUNCTION()
