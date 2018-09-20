project (monya)

include_directories("${PROJECT_SOURCE_DIR}")

add_executable(kdtree
        kdtree.cpp
        )

target_link_libraries(kdtree stdc++ monya)
