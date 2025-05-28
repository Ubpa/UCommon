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
  list(APPEND defines "UBPA_UCOMMON_USING_SHARED_LIBRARY")
  list(APPEND private_defines "UBPA_UCOMMON_BUILDING_SHARED_LIBRARY")
else()
  set(mode STATIC)
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
    "${PROJECT_SOURCE_DIR}/include/UCommon_ext/Tex2DIO.h"
  LIB
    Ubpa::UCommon_Runtime
    Ubpa::UCommon_ext_stb
  C_OPTION
    ${c_options}
)
