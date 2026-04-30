CU_SET_PATH("CMAKE_MOD_CROSSROADS_EVENT_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/crossroads_event.cpp")
AC_ADD_SCRIPT_LOADER("CrossroadsEvent" "${CMAKE_CURRENT_LIST_DIR}/src/mod_crossroads_event_loader.cpp")