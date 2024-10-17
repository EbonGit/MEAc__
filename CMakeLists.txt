cmake_minimum_required(VERSION 3.28)
project(MEAc__)

set(CMAKE_CXX_STANDARD 20)

set(OpenCV_DIR C:/OpenCV/opencv/build/x64/vc16/lib)

# Find OpenCV package
find_package(OpenCV REQUIRED)

add_subdirectory(pybind11)

find_package(Python3 COMPONENTS Interpreter Development)

# Add the executable
add_executable(MEAc__ src/main.cpp
        src/MEA.cpp
        src/MEA.h
        src/utils.cpp
        src/utils.h
        src/Vue.cpp
        src/Vue.h
        src/Data.cpp
        src/Data.h
        src/PythonAPI.cpp
        src/PythonAPI.h
        src/config.h
        src/Network.cpp
        src/Network.h
        src/VecFloat.h
        src/stack.h
        src/tcp.cpp
        src/tcp.h)

# Link OpenCV libraries
target_link_libraries(MEAc__ PRIVATE ${OpenCV_LIBS})
target_link_libraries(MEAc__ PRIVATE pybind11::embed Python3::Python)

add_custom_command(TARGET MEAc__ POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:MEAc__> ${CMAKE_CURRENT_BINARY_DIR}/../build)

add_executable(TCPConsole src/dummy/tcp_dummy.h
                    src/dummy/tcp_dummy.cpp
                    src/dummy/main.cpp)
