# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(sqlitecpp_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(sqlitecpp_FRAMEWORKS_FOUND_RELEASE "${sqlitecpp_FRAMEWORKS_RELEASE}" "${sqlitecpp_FRAMEWORK_DIRS_RELEASE}")

set(sqlitecpp_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET sqlitecpp_DEPS_TARGET)
    add_library(sqlitecpp_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET sqlitecpp_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${sqlitecpp_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${sqlitecpp_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:SQLite::SQLite3>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### sqlitecpp_DEPS_TARGET to all of them
conan_package_library_targets("${sqlitecpp_LIBS_RELEASE}"    # libraries
                              "${sqlitecpp_LIB_DIRS_RELEASE}" # package_libdir
                              "${sqlitecpp_BIN_DIRS_RELEASE}" # package_bindir
                              "${sqlitecpp_LIBRARY_TYPE_RELEASE}"
                              "${sqlitecpp_IS_HOST_WINDOWS_RELEASE}"
                              sqlitecpp_DEPS_TARGET
                              sqlitecpp_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "sqlitecpp"    # package_name
                              "${sqlitecpp_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${sqlitecpp_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${sqlitecpp_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${sqlitecpp_LIBRARIES_TARGETS}>
                 )

    if("${sqlitecpp_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET SQLiteCpp
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     sqlitecpp_DEPS_TARGET)
    endif()

    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${sqlitecpp_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${sqlitecpp_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${sqlitecpp_LIB_DIRS_RELEASE}>)
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${sqlitecpp_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${sqlitecpp_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(sqlitecpp_LIBRARIES_RELEASE SQLiteCpp)
