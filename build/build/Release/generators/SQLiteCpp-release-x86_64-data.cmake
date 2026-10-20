########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(sqlitecpp_COMPONENT_NAMES "")
if(DEFINED sqlitecpp_FIND_DEPENDENCY_NAMES)
  list(APPEND sqlitecpp_FIND_DEPENDENCY_NAMES SQLite3)
  list(REMOVE_DUPLICATES sqlitecpp_FIND_DEPENDENCY_NAMES)
else()
  set(sqlitecpp_FIND_DEPENDENCY_NAMES SQLite3)
endif()
set(SQLite3_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(sqlitecpp_PACKAGE_FOLDER_RELEASE "/home/joshd/.conan2/p/b/sqlit53919f9fd28bf/p")
set(sqlitecpp_BUILD_MODULES_PATHS_RELEASE )


set(sqlitecpp_INCLUDE_DIRS_RELEASE "${sqlitecpp_PACKAGE_FOLDER_RELEASE}/include")
set(sqlitecpp_RES_DIRS_RELEASE )
set(sqlitecpp_DEFINITIONS_RELEASE )
set(sqlitecpp_SHARED_LINK_FLAGS_RELEASE )
set(sqlitecpp_EXE_LINK_FLAGS_RELEASE )
set(sqlitecpp_OBJECTS_RELEASE )
set(sqlitecpp_COMPILE_DEFINITIONS_RELEASE )
set(sqlitecpp_COMPILE_OPTIONS_C_RELEASE )
set(sqlitecpp_COMPILE_OPTIONS_CXX_RELEASE )
set(sqlitecpp_LIB_DIRS_RELEASE "${sqlitecpp_PACKAGE_FOLDER_RELEASE}/lib")
set(sqlitecpp_BIN_DIRS_RELEASE )
set(sqlitecpp_LIBRARY_TYPE_RELEASE STATIC)
set(sqlitecpp_IS_HOST_WINDOWS_RELEASE 0)
set(sqlitecpp_LIBS_RELEASE SQLiteCpp)
set(sqlitecpp_SYSTEM_LIBS_RELEASE pthread dl m)
set(sqlitecpp_FRAMEWORK_DIRS_RELEASE )
set(sqlitecpp_FRAMEWORKS_RELEASE )
set(sqlitecpp_BUILD_DIRS_RELEASE )
set(sqlitecpp_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(sqlitecpp_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${sqlitecpp_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${sqlitecpp_COMPILE_OPTIONS_C_RELEASE}>")
set(sqlitecpp_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${sqlitecpp_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${sqlitecpp_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${sqlitecpp_EXE_LINK_FLAGS_RELEASE}>")


set(sqlitecpp_COMPONENTS_RELEASE )