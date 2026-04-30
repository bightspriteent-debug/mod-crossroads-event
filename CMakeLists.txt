CU_ADD_HOOK(AFTER_LOAD_CONF
    "${CMAKE_CURRENT_LIST_DIR}/src/mod_crossroads_event_loader.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/crossroads_event.cpp"
)