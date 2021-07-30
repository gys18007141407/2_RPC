add_compile_options(-std=c++11 -O0 -ggdb)

aux_source_directory(. SRC_CPP)

add_library(simpleRPC ${SRC_CPP})

target_link_libraries(simpleRPC muduo_net muduo_base pthread zookeeper_mt)