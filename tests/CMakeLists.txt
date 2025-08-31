
add_executable(test_logger test_logger.cpp)
target_include_directories(test_logger PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_logger PUBLIC logger)
