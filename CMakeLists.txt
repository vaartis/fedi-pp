cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(fedi-pp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC TRUE)

set(fedi-pp-src
  src/qml/main.qrc
  src/main.cpp

  src/Client.hpp

  src/TimelinePostModel.hpp
)

add_executable(fedi-pp ${fedi-pp-src})
target_include_directories(fedi-pp PUBLIC "src/")

find_package(Qt5 COMPONENTS Widgets Quick QuickControls2 REQUIRED)
target_link_libraries(fedi-pp Qt5::Widgets Qt5::Quick Qt5::QuickControls2)

find_package(KF5Wallet REQUIRED)
target_link_libraries(fedi-pp KF5::Wallet)

find_package(KF5Plasma REQUIRED)
target_link_libraries(fedi-pp KF5::Plasma)
