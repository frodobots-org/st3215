cmake_minimum_required(VERSION 2.4)
project(http_parser)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

file(GLOB_RECURSE ${PROJECT_NAME}_files
    ${PROJECT_SOURCE_DIR}/src/*.c    
    )

MESSAGE(project_files " " ${${PROJECT_NAME}_files})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/${MACHINE})

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_files})
