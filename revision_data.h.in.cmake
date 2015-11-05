#ifndef __REVISION_DATA_H__
#define __REVISION_DATA_H__
 #define _HASH                      "@rev_hash@"
 #define _DATE                      "@rev_date@"
 #define _BRANCH                    "@rev_branch@"
 #define _BUILD_NUM					"@rev_number@"
 #define _SOURCE_DIRECTORY          "@CMAKE_SOURCE_DIR@"
 #define _MYSQL_EXECUTABLE          "@MYSQL_EXECUTABLE@"
 #define _FULL_DATABASE             "TDB_full_.sql"
 #define _HOTFIXES_DATABASE         "TDB_full_hotfixes.sql"
 #define VER_COMPANYNAME_STR        "Tornado Developers"
 #define VER_LEGALCOPYRIGHT_STR     "(c)2008-2015 Tornado"
 #define VER_FILEVERSION            0,0,0
 #define VER_FILEVERSION_STR        "@rev_hash@ @rev_date@ (@rev_branch@ branch)"
 #define VER_PRODUCTVERSION         VER_FILEVERSION
 #define VER_PRODUCTVERSION_STR     VER_FILEVERSION_STR
 #define COMPILER_C_FLAGS           "@CMAKE_C_FLAGS@"
 #define COMPILER_CXX_FLAGS         "@CMAKE_CXX_FLAGS@"
#endif // __REVISION_DATA_H__