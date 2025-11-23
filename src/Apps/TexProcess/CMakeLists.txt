set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd4251")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #
endif()

Ubpa_AddTarget(
  MODE EXE
  CXX_STANDARD 17
  LIB
    Ubpa::UCommon_Runtime
    Ubpa::UCommon_ext_Tex2DIO
  C_OPTION
    ${c_options} 
)

