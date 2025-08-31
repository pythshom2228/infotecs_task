cmake_minimum_required(VERSION 3.10)
project(TestTask CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# По умолчанию собираем динамическую библиотеку
option(BUILD_SHARED_LIBS  "Build using shared libraries" ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()


if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options(-Wall -Wextra -Wpedantic -O0 -g)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

set(LOGGER_SRC ${CMAKE_SOURCE_DIR}/src/Logger)
set(LOGAPP_SRC ${CMAKE_SOURCE_DIR}/src/LogApp)
set(RECEIVER_SRC ${CMAKE_SOURCE_DIR}/src/Receiver)


# Библиотека 
add_library( logger 
    ${LOGGER_SRC}/logger.cpp
    ${LOGGER_SRC}/file_logger.cpp
    ${LOGGER_SRC}/socket_logger.cpp
    ${LOGGER_SRC}/file_sink.cpp
    ${LOGGER_SRC}/socket_sink.cpp
    
)
target_include_directories(logger PUBLIC ${CMAKE_SOURCE_DIR}/include)


# Приложение часть 2
add_executable(log_app 
    ${LOGAPP_SRC}/log_app.cpp
)
target_link_libraries(log_app PUBLIC logger)

# Приложение часть 3
add_executable(receiver 
    ${RECEIVER_SRC}/receiver.cpp
    ${RECEIVER_SRC}/log_server.cpp
    ${RECEIVER_SRC}/stats_collector.cpp
)
target_include_directories(receiver PUBLIC ${CMAKE_SOURCE_DIR}/include)

#  Тесты
add_subdirectory(tests)
