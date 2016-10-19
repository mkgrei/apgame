cmake_minimum_required(VERSION 2.7)

add_executable(apgame-example-server server.cpp)
target_link_libraries(apgame-example-server boost_date_time boost_system boost_coroutine boost_context boost_program_options pthread)

add_executable(apgame-example-client client.cpp)
target_link_libraries(apgame-example-client boost_date_time boost_system boost_coroutine boost_context boost_program_options pthread)

