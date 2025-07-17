# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\lgsf_login_scratch_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\lgsf_login_scratch_autogen.dir\\ParseCache.txt"
  "lgsf_login_scratch_autogen"
  )
endif()
