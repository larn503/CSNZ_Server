project(test_net)

add_executable(test_net)

target_sources(test_net PRIVATE "net.cpp")

target_link_libraries(test_net PRIVATE net)
