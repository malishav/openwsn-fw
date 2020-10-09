add_library(web STATIC "")

target_sources(web
        PUBLIC
        coap/coap.c
        coap/coap.h
        coap/oscore.c
        coap/oscore.h
        coap/cborencoder.c
        coap/cborencoder.h
        web.c
        web.h)

target_compile_features(web PUBLIC c_std_99)
target_include_directories(web PUBLIC "." "coap")

target_link_libraries(web PRIVATE bsp)
target_link_libraries(web PRIVATE drivers)
target_link_libraries(web PRIVATE stack)
