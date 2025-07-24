# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/MainpageTest_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/MainpageTest_autogen.dir/ParseCache.txt"
  "MainpageTest_autogen"
  )
endif()
