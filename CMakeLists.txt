cmake_minimum_required(VERSION 3.10)
project(TrainDispatchSystem)

set(CMAKE_CXX_STANDARD 17)

# Thêm đường dẫn include của MinGW UCRT64 (SFML, Raylib, ...)
include_directories(
    include
    C:/msys64/ucrt64/include
)

link_directories(
    C:/msys64/ucrt64/lib
)

file(GLOB SOURCES "src/*.cpp")

add_executable(TrainDispatchSystem ${SOURCES})

# Liên kết Raylib và các thư viện phụ thuộc Windows
target_link_libraries(TrainDispatchSystem
    raylib
    opengl32
    gdi32
    winmm
)
