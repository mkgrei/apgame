cmake_minimum_required(VERSION 2.7)

add_executable(apgame-server server.cpp)
target_link_libraries(apgame-server boost_date_time boost_system boost_coroutine pthread)

add_executable(apgame-client client.cpp)
target_link_libraries(apgame-client boost_date_time boost_system boost_coroutine)
