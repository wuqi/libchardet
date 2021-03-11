cmake_minimum_required ( VERSION 3.0)

project(chardet)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src chardet_src)

add_library (${PROJECT_NAME} ${chardet_src})
