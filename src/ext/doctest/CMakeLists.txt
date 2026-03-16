Ubpa_DownloadFile(
  https://udata-1308066321.cos.ap-guangzhou.myqcloud.com/doctest_1da23a3.h
  ${PROJECT_SOURCE_DIR}/include/UCommon_ext/doctest/doctest.h
  SHA256 94029a7d32da24a56249658147dbd2b33ff0b9ed665295cbbaf19aafff5b0ced
)

set(c_options "")
if(MSVC)
  list(APPEND c_options
    "/wd4251"
    "/wd4334"
    "/wd4267"
    "/wd4514"
    "/wd4710"
    "/wd4711")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #
endif()

set(mode "")
set(defines "")
set(private_defines "")
if(Ubpa_UCommon_UsingSharedLibrary)
  set(mode SHARED)
  list(APPEND defines "UBPA_UCOMMON_USING_SHARED_LIBRARY" "DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL")
  list(APPEND private_defines "UBPA_UCOMMON_BUILDING_SHARED_LIBRARY" "DOCTEST_CONFIG_IMPLEMENT")
else()
  set(mode STATIC)
  list(APPEND private_defines "DOCTEST_CONFIG_IMPLEMENT")
endif()

Ubpa_AddTarget(
  MODE ${mode}
  CXX_STANDARD 17
  DEFINE
    ${defines}
  PRIVATE_DEFINE
    ${private_defines}
  INC
    "${PROJECT_SOURCE_DIR}/include"
  SOURCE
    "${PROJECT_SOURCE_DIR}/include/UCommon_ext/doctest"
  C_OPTION
    ${c_options}
)
