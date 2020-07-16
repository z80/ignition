
#ifndef __FRAME_SELECTOR_H_
#define __FRAME_SELECTOR_H_

#include <vector>
#include "indexed_kd_tree.hpp"

class FrameSelector
{
public:
    constexpr static size_t DIMS = 18;

    typedef double Float;
    typedef std::vector<Float> Vector;
    typedef KdTree::IndexedKdTree<DIMS>::Tree Tree;

    // Small value to compare STD with zero.
    constexpr static Float EPS = 1.0e-19;


    FrameSelector();
    ~FrameSelector();

    void clear();
    void reserve( size_t qty );
    void append( const std::vector<double> & vals );
    int size() const;
    // This one comutes mean and standard deviation for scaling.
    void computeMeanStd();
    // And this one instead of standard deviation computes amplitude.
    // Uses amplitude*cutoff as a scale factor.
    void computeMeanAmp( double cutoff=0.9 );
    void normalize();
    void overrideVal( size_t nodeInd, size_t valInd, Float val );
    void buildTree();

    // For debugging.
    const Vector & mean() const;
    const Vector & invStd() const; 

    // Can override "mean" and "std" in order to 
    // be able to make certain parts more or less important.
    void setMean( int ind, double v );
    void setInvStd( int ind, double v );

    // Queries.
    size_t closestFrame( const std::vector<double> & frame );
    double closestDistance();
    double distanceToNode( const std::vector<double> & frame, size_t nodeInd );

    void setDebug( bool en );
private:
    Vector mean_;
    Vector inv_std_;
    Tree   tree_;
};





#endif







