cmake_minimum_required(VERSION 3.16)

include_directories(inc)

project(objectsDetecting)

find_package(ImageMagick COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} 
               src/GaussSignal.cpp
               src/RealSignal.cpp
               src/Signal.cpp
               src/SignalFunctions.cpp
               src/TestSignal.cpp
               main.cpp)

target_link_libraries(${PROJECT_NAME} ${ImageMagick_LIBRARIES})