#ifndef TREE_PARTITION_HPP__
#define TREE_PARTITION_HPP__

#include <memory>

namespace monya { namespace container {

template <typename NodeType>
class TreePartition {

    private:
        short numa_id;
        size_t id;
        NodeType root;

        TreePartition() {
        }

    public:
        typedef std::shared_ptr<TreePartition<NodeType> > ptr;
};

} } // End monya::container

#endif
