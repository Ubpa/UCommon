set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd4251")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #
endif()

Ubpa_AddTarget(
  TEST
  MODE EXE
  LIB
    Ubpa::UCommon_Core
  C_OPTION
    ${c_options} 
)
