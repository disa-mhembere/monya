project (monya)

include_directories("${PROJECT_SOURCE_DIR}")

add_library(monya STATIC
    BinaryTreeProgram.cpp
)

subdirs(
    common
    structures
    io
    utils
    validate
    examples
)
