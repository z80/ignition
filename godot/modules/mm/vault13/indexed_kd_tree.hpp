
#ifndef __INDEXED_KD_TREE_HPP_
#define __INDEXED_KD_TREE_HPP_

#include "kd_tree.hpp"

namespace KdTree
{

template<size_t N>
struct IndexedKdTree
{
    typedef kdtree<double, N, size_t> Tree;
};

}



#endif







