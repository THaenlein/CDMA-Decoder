###############################################################################
## Add common header files
LIST(APPEND HEADERFILES
  "${CMAKE_CURRENT_LIST_DIR}/main.h"
  "${CMAKE_CURRENT_LIST_DIR}/SequenceGenerator.h"
  "${CMAKE_CURRENT_LIST_DIR}/Decoder.h"
  "${CMAKE_CURRENT_LIST_DIR}/stdafx.h"
)

###############################################################################
## Add common source files
LIST(APPEND SOURCEFILES
  "${CMAKE_CURRENT_LIST_DIR}/main.c"
  "${CMAKE_CURRENT_LIST_DIR}/SequenceGenerator.c"
  "${CMAKE_CURRENT_LIST_DIR}/Decoder.c"
)

add_executable("${PROJECT_NAME}" ${SOURCEFILES} ${HEADERFILES})