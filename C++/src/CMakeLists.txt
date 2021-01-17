###############################################################################
## Add common header files
LIST(APPEND HEADERFILES
  "${CMAKE_CURRENT_LIST_DIR}/main.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/SequenceGenerator.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/Decoder.hpp"
)

###############################################################################
## Add common source files
LIST(APPEND SOURCEFILES
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SequenceGenerator.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Decoder.cpp"
)

add_executable("${PROJECT_NAME}" ${SOURCEFILES} ${HEADERFILES})