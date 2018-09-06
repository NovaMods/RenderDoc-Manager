cmake_minimum_required(VERSION 3.8)
project(RenderDocApi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(SOURCE_FILES
        renderdoc_app.h
        RenderDocManager.cpp
        RenderDocManager.h)

set(HEADERS
        RenderDocManager.h)

add_library(RenderDocApi SHARED ${SOURCE_FILES})

# Install config
set(include_dest "${CMAKE_INSTALL_PREFIX}/include/RenderDocApi")
set(main_lib_dest "${CMAKE_INSTALL_PREFIX}/lib/RenderDocApi")

install(TARGETS RenderDocApi DESTINATION "${main_lib_dest}")
install(FILES ${HEADERS} DESTINATION "${include_dest}")
