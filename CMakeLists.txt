cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(UCommon VERSION 1.4.1)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake(VERSION 0.6.4)

Ubpa_InitProject()

Ubpa_DownloadFile(
  https://udata-1308066321.cos.ap-guangzhou.myqcloud.com/half.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/UCommon/_deps/half.hpp
  SHA256 8c084b0b6fa4fab6aef7173ae99a80a87a879fbdd1ce0a5cdf4620319995171a
)

Ubpa_AddSubDirsRec(src)

Ubpa_Export(
  TARGET
  DIRECTORIES
    "include"
)
