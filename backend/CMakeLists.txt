### configure crow (static file serving)
get_filename_component(CROW_STATIC_DIRECTORY "${PROJECT_SOURCE_DIR}/ui/dist" ABSOLUTE)
add_definitions(-DCROW_STATIC_DIRECTORY="${CROW_STATIC_DIRECTORY}")
message("ui-dir: ${CROW_STATIC_DIRECTORY}")
add_definitions(-DCROW_STATIC_ENDPOINT="/<path>")

add_definitions(-DCROW_ENFORCE_WS_SPEC) # https://crowcpp.org/1.0/guides/websockets/

set(EXENAME backend)
add_executable(${EXENAME} main.cpp)
target_link_libraries(${EXENAME} Crow::Crow)
add_dependencies(${EXENAME} webui)