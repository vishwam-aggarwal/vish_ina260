add_library( vish_ina260
    vish_ina260.h
    vish_ina260.cpp
    )

add_subdirectory(vish_busio)

target_link_libraries(vish_ina260
    vish_busio
)

target_include_directories(vish_ina260 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
