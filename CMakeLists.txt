cmake_minimum_required(VERSION 2.8)

project(spark_demo)

add_definitions(-DGL_GLEXT_PROTOTYPES)

message("SYSTEM NAME : ${CMAKE_SYSTEM_NAME}")
message("SYSTEM VERSION : ${CMAKE_SYSTEM_VERSION}")
message("SYSTEM PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")

AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Core" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Extensions/Emitters" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Extensions/Modifiers" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Extensions/Renderers" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Extensions/Zones" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/RenderingAPIs/OpenGLES" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src" spark_demo_app_SRC)

include_directories ("${PROJECT_SOURCE_DIR}/target/qnx7/usr/include")
include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${PROJECT_SOURCE_DIR}/third_party/glm/include")
include_directories ("${PROJECT_SOURCE_DIR}/third_party/glad/include")
include_directories ("${PROJECT_SOURCE_DIR}/src")

ADD_LIBRARY(spark_demo_app STATIC ${spark_demo_app_SRC})

target_link_libraries(spark_demo_app /usr/lib/x86_64-linux-gnu/libGLEW.so)
target_link_libraries(spark_demo_app /usr/lib/x86_64-linux-gnu/libGLESv2.so)
