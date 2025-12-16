Ubpa_DownloadZip_Pro(
  https://utencent-1308066321.cos.ap-guangzhou.myqcloud.com/astcenc_3efe15_20251216.zip
  astcenc.zip
  "${CMAKE_CURRENT_SOURCE_DIR}/_deps"
  SHA256 d194a6730176de5f4ca32f2e79d4e1d9be8f33b850d0cf394dc3515f76091bc8
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
if(MSVC)
  list(APPEND private_defines
    "_CRT_SECURE_NO_WARNINGS")
  list(APPEND defines
    "__SSE4_2__")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #
endif()
if(Ubpa_UMBD_UsingSharedLibrary)
  set(mode SHARED)
  list(APPEND defines "UBPA_UMBD_USING_SHARED_LIBRARY")
  list(APPEND private_defines "UBPA_UMBD_BUILDING_SHARED_LIBRARY")
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
  SOURCE
    "${PROJECT_SOURCE_DIR}/include/UCommon_ext/ASTCUtils.h"
  INC_PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/_deps"
    "${CMAKE_CURRENT_SOURCE_DIR}/_deps/astcenc"
  C_OPTION
    ${c_options}
  LIB
    Ubpa::UCommon_Runtime
)
