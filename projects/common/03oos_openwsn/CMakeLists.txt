add_executable(${PROJECT} "")

target_sources(${PROJECT} PUBLIC openwsn.c openwsn.h)

target_link_libraries(${PROJECT} PUBLIC bsp)
target_link_libraries(${PROJECT} PUBLIC kernel)
target_link_libraries(${PROJECT} PUBLIC stack)

