
#ifndef __KD_TREE_HPP_
#define __KD_TREE_HPP_


#include <algorithm>
#include <array>
#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>

namespace KdTree
{

 
/**
 * Class for representing a point. coordinate_type must be a numeric type.
 */
template<typename coordinate_type, size_t dimensions, typename object_type=size_t>
class point
{
public:
    point( const std::array<coordinate_type, dimensions> & c )
        : coords_( c )
    {
    }
    point( const std::array<coordinate_type, dimensions> & c, const object_type & o )
        : coords_( c ), 
          object_( o )
    {
    }
    point( const std::vector<coordinate_type> & c, const object_type & o )
        : object_( o )
    {
        // Fill all the coordinates with 0 in case fewer elements than 
        // dinemsionality is provided.
        const coordinate_type _0 = static_cast<coordinate_type>( 0 );
        coords_.fill( _0 );
        // Use provided coordinated to get significant values.
        size_t n = std::min(dimensions, c.size());
        std::copy_n( c.begin(), n, coords_.begin() );

    }

    const point & operator=( const point & inst )
    {
        if ( this != &inst )
        {
            coords_ = inst.coords_;
            object_ = inst.object_;
        }
        return *this;
    }
    point( const std::initializer_list<coordinate_type> & list )
    {
        // Fill all the coordinates with 0 in case lewer elements than 
        // donemsionality is provided.
        const coordinate_type _0 = static_cast<coordinate_type>( 0 );
        coords_.fill( _0 );
        // Use provided coordinated to get significant values.
        size_t n = std::min(dimensions, list.size());
        std::copy_n(list.begin(), n, coords_.begin());
    }
    /**
     * Returns the coordinate in the given dimension.
     *
     * @param index dimension index (zero based)
     * @return coordinate in the given dimension
     */
    const coordinate_type & get(size_t index) const
    {
        return coords_[index];
    }
    coordinate_type & get(size_t index)
    {
        return coords_[index];
    }

    /**
     * Returns the distance squared from this point to another
     * point.
     *
     * @param pt another point
     * @return distance squared from this point to the other point
     */
    double distance( const point & pt, bool debug_output=false ) const
    {
        double dist = 0;
        for (size_t i = 0; i < dimensions; ++i)
        {
            const coordinate_type own = get(i);
            const coordinate_type other = pt.get(i);
            const double d = own - other;
            dist += d * d;
            if ( debug_output )
            {
                std::cout << d << " = " << own << " - " << other << ", accum = " << dist << std::endl;
            }
        }
        return dist;
    }
private:
    std::array<coordinate_type, dimensions> coords_;
public:
    object_type object_;
};
 
template<typename coordinate_type, size_t dimensions>
std::ostream& operator<<( std::ostream & out, const point<coordinate_type, dimensions> & pt )
{
    out << '(';
    for (size_t i = 0; i < dimensions; ++i)
    {
        if (i > 0)
            out << ", ";
        out << pt.get(i);
    }
    out << ')';
    return out;
}
 
/**
 * C++ k-d tree implementation, based on the C version at rosettacode.org.
 */
template<typename coordinate_type, size_t dimensions, typename object_type=size_t>
class kdtree
{
public:
    typedef point<coordinate_type, dimensions, object_type> point_type;
private:
    struct node
    {
        node(const point_type& pt)
            : point_(pt), 
              left_(nullptr), 
              right_(nullptr)
        {
        }
        coordinate_type get(size_t index) const
        {
            return point_.get(index);
        }
        double distance(const point_type& pt) const
        {
            return point_.distance(pt);
        }
        point_type point_;
        node* left_;
        node* right_;
    };
    node* root_;
    node* best_;
    double best_dist_;
    size_t visited_;
    std::vector<node> nodes_;
    std::vector<object_type> indices_;
    bool debug_output_;
 
    struct node_cmp
    {
        node_cmp(size_t index) : index_(index)
        {
        }
        bool operator()(const node& n1, const node& n2) const
        {
            return n1.point_.get(index_) < n2.point_.get(index_);
        }
        size_t index_;
    };
 
    node* make_tree(size_t begin, size_t end, size_t index)
    {
        if (end <= begin)
            return nullptr;
        size_t n = begin + (end - begin)/2;
        std::nth_element(&nodes_[begin], &nodes_[n], &nodes_[end], node_cmp(index));
        index = (index + 1) % dimensions;
        nodes_[n].left_ = make_tree(begin, n, index);
        nodes_[n].right_ = make_tree(n + 1, end, index);
        return &nodes_[n];
    }
 
    void nearest(node* root, const point_type& point, size_t index)
    {
        if (root == nullptr)
            return;
        ++visited_;
        double d = root->distance(point);
        if (best_ == nullptr || d < best_dist_)
        {
            best_dist_ = d;
            best_ = root;
        }
        if (best_dist_ == 0)
            return;
        double dx = root->get(index) - point.get(index);
        index = (index + 1) % dimensions;
        const bool left_is_wrong = ( root->left_ ) ? in_wrong_area( root->left_->point_, point ) : true;
        const bool right_is_wrong = ( root->right_ ) ? in_wrong_area( root->left_->point_, point ) : true;
        const bool do_left_first = (dx > 0);
        const coordinate_type dx2 = dx*dx;

        if ( do_left_first )
        {
            if ( !left_is_wrong )
            {
                nearest( root->left_, point, index );
                if ( dx2 >= best_dist_ )
                    return;
                if ( !right_is_wrong )
                    nearest( root->right_, point, index );
            }
            else
            {
                if ( !right_is_wrong )
                    nearest( root->right_, point, index );
                else
                {
                    nearest( root->left_, point, index );
                    if ( dx2 >= best_dist_ )
                        return;
                    nearest( root->right_, point, index );

                }
            }
        }
        else
        {
            if ( !right_is_wrong )
            {
                nearest( root->right_, point, index );
                if ( dx2 >= best_dist_ )
                    return;
                if ( !left_is_wrong )
                    nearest( root->left_, point, index );
            }
            else
            {
                if ( !left_is_wrong )
                    nearest( root->left_, point, index );
                else
                {
                    nearest( root->right_, point, index );
                    if ( dx2 >= best_dist_ )
                        return;
                    nearest( root->left_, point, index );

                }
            }
        }
    }
public:
    kdtree(const kdtree&) = delete;
    kdtree& operator=(const kdtree&) = delete;
    /**
     * Constructor taking a pair of iterators. Adds each
     * point in the range [begin, end) to the tree.
     *
     * @param begin start of range
     * @param end end of range
     */
    template<typename iterator>
    kdtree(iterator begin, iterator end)
    {
        best_ = nullptr;
        best_dist_ = 0;
        visited_ = 0;
        nodes_.reserve(std::distance(begin, end));
        for (auto i = begin; i != end; ++i)
            nodes_.emplace_back(*i);
        root_ = make_tree(0, nodes_.size(), 0);
    }
 
    /**
     * Constructor taking a function object that generates
     * points. The function object will be called n times
     * to populate the tree.
     *
     * @param f function that returns a point
     * @param n number of points to add
     */
    template<typename func>
    kdtree(func&& f, size_t n)
    {
        best_ = nullptr;
        best_dist_ = 0;
        visited_ = 0;
        nodes_.reserve(n);
        for (size_t i = 0; i < n; ++i)
            nodes_.emplace_back(f());
        root_ = make_tree(0, nodes_.size(), 0);
        build_indices();
    }

    kdtree()
    {
        best_ = nullptr;
        best_dist_ = 0;
        root_ = nullptr;
        debug_output_ = false;
    }

    void set_debug_output( bool en )
    {
        debug_output_ = en;
    }
    bool debug_output() const
    {
        return debug_output_;
    }
    void build_indices()
    {
        const size_t qty = nodes_.size();
        indices_.resize( qty );
        for ( size_t i=0; i<qty; i++ )
        {
            const node & n = nodes_[i];
            const object_type & v = n.point_.object_;
            indices_[v] = i;
        }
    }
    void clear()
    {
        best_ = nullptr;
        best_dist_ = 0;
        root_ = nullptr;
        nodes_.clear();
        indices_.clear();
    }
    size_t size() const
    {
        const size_t qty = nodes_.size();
        return qty;
    }
    void reserve( size_t qty )
    {
        nodes_.reserve( qty );
    }
    void append( const node & n )
    {
        //std::cout << "kd_tree::append()" << std::endl;
        //std::cout << "nodes_.size() before: " << nodes_.size() << std::endl;
        nodes_.push_back( n );
        //std::cout << "nodes_.size() after: " << nodes_.size() << std::endl;
    }
    coordinate_type & val( size_t nodeInd, size_t valInd )
    {
        node & n = nodes_[nodeInd];
        coordinate_type & v = n.point_.get( valInd );
        return v;
    }
    const coordinate_type & val( size_t nodeInd, size_t valInd ) const
    {
        const node & n = nodes_[nodeInd];
        const coordinate_type & v = n.point_.get( valInd );
        return v;
    }
    void build_tree()
    {
        best_ = nullptr;
        best_dist_ = 0;
        root_ = nullptr;
        root_ = make_tree( 0, nodes_.size(), 0 );
        build_indices();
    }
 
    /**
     * Returns true if the tree is empty, false otherwise.
     */
    bool empty() const
    {
        return nodes_.empty();
    }
 
    /**
     * Returns the number of nodes visited by the last call
     * to nearest().
     */
    size_t visited() const
    {
        return visited_;
    }
 
    /**
     * Returns the distance between the input point and return value
     * from the last call to nearest().
     */
    double distance() const
    {
        return std::sqrt(best_dist_);
    }

    double distance_to_node( const point_type & pt, size_t ind )
    {
        const size_t node_ind = indices_[ind];
        const point_type & tree_pt = nodes_[node_ind].point_;
        if ( debug_output_ )
            std::cout << "distance to node #" << ind << ", node object #" << tree_pt.object_ << std::endl;
        const double res2 = pt.distance( tree_pt, debug_output_ );
        const double res = std::sqrt(res2);
        return res;
    }
 
    /**
     * Finds the nearest point in the tree to the given point.
     * It is not valid to call this function if the tree is empty.
     *
     * @param pt a point
     * @param the nearest point in the tree to the given point
     */
    const point_type & nearest( const point_type & pt )
    {
        if (root_ == nullptr)
            throw std::logic_error("tree is empty");
        best_ = nullptr;
        visited_ = 0;
        best_dist_ = 0;
        nearest(root_, pt, 0);
        return best_->point_;
    }
    const object_type & nearest_object( const point_type & pt )
    {
        if (root_ == nullptr)
            throw std::logic_error("tree is empty");
        best_ = nullptr;
        visited_ = 0;
        best_dist_ = 0;
        nearest(root_, pt, 0);
        if ( debug_output_ )
        {
            std::ostringstream out;
            out << "nearest obj #" << best_->point_.object_ << ": (";
            for ( size_t i=0; i<dimensions; i++ )
            {
                const coordinate_type v = best_->point_.get(i);
                out << v << ", ";
            }
            out << ")";
            std::cout << out.str() << std::endl;
        }
        return best_->point_.object_;
    }

    virtual bool in_wrong_area( const point_type & node_pt, const point_type & pt ) const
    {
        return false;
    }
};
 


} // namespace KdTree

#endif




