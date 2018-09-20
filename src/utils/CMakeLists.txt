project (monya)

include_directories("${PROJECT_SOURCE_DIR}")

add_library(utils STATIC
    FileUtil.cpp
    time.cpp
    utility.cpp
)
