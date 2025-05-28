Ubpa_DownloadFile(
  https://udata-1308066321.cos.ap-guangzhou.myqcloud.com/stb_image_v2_30.h
  ${CMAKE_PROJECT_SOURCE_DIR}/include/UCommon_ext/stb/stb_image.h
  SHA256 594c2fe35d49488b4382dbfaec8f98366defca819d916ac95becf3e75f4200b3
)

Ubpa_DownloadFile(
  https://udata-1308066321.cos.ap-guangzhou.myqcloud.com/stb_image_write_v1_16.h
  ${CMAKE_PROJECT_SOURCE_DIR}/include/UCommon_ext/stb/stb_image_write.h
  SHA256 cbd5f0ad7a9cf4468affb36354a1d2338034f2c12473cf1a8e32053cb6914a05
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
    "${PROJECT_SOURCE_DIR}/include/UCommon_ext/stb"
  C_OPTION
    ${c_options}
)
