cmake_minimum_required(VERSION 2.7)

add_executable(apgame-server apgame-server.cpp)
target_link_libraries(apgame-server boost_date_time boost_system boost_coroutine boost_context boost_program_options pthread)

add_executable(apgame-client apgame-client.cpp)
target_link_libraries(apgame-client lh-ai boost_date_time boost_system boost_coroutine boost_context boost_program_options)

add_executable(client-human client-human.cpp)
target_link_libraries(client-human boost_date_time boost_system boost_coroutine boost_context boost_program_options)
