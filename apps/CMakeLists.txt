add_library(apps STATIC "")

target_sources(apps
        PUBLIC
        apps.c
        uecho/uecho.h
        uecho/uecho.c
        cjoin/cjoin.h
        cjoin/cojp_cbor.h
        cjoin/cojp_cbor.c
        cjoin/cjoin.c)

target_compile_features(apps PUBLIC c_std_99)
target_include_directories(apps
        PUBLIC
        "."
        "uecho"
        "cjoin")
target_link_libraries(apps PRIVATE bsp)
target_link_libraries(apps PRIVATE web)
target_link_libraries(apps PRIVATE stack)
target_link_libraries(apps PRIVATE drivers)
