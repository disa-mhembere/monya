project (monya)

include_directories("${PROJECT_SOURCE_DIR}")

add_library(structures STATIC
    BinaryTree.cpp
    NAryNode.cpp
    NodeView.cpp
    Query.cpp
    RBTree.cpp
    Scheduler.cpp
    TaskQueue.cpp
    VecBinaryTree.cpp
    WorkerThread.cpp
)
