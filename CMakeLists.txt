cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(UCommon VERSION 3.0.0)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake(VERSION 0.7.3)

Ubpa_InitProject()

Ubpa_DownloadFile(
  https://udata-1308066321.cos.ap-guangzhou.myqcloud.com/half.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/UCommon/_deps/half.hpp
  SHA256 8c084b0b6fa4fab6aef7173ae99a80a87a879fbdd1ce0a5cdf4620319995171a
)

option(Ubpa_UCommon_UsingSharedLibrary "using shared library" OFF)

Ubpa_AddSubDirsRec(src)

add_custom_target(check
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config $<CONFIG> --target UCommon_BuildTests
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Building all test targets and running tests..."
)

Ubpa_Export(
  TARGET
  DIRECTORIES
    "include"
)
