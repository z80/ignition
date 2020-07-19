

#include "frame_selector.h"


using namespace KdTree;


FrameSelector::FrameSelector()
{
}

FrameSelector::~FrameSelector()
{
}

void FrameSelector::clear()
{
    tree_.clear();
}

void FrameSelector::reserve( size_t qty )
{
    tree_.reserve( qty );
}

void FrameSelector::append( const std::vector<double> & vals )
{
    //std::cout << "Entered \'append()\'" << std::endl;

    const size_t index = tree_.size();
    const point<Float, DIMS, size_t> n( vals, index );
    tree_.append( n );
}

int FrameSelector::size() const
{
    //std::cout << "Entered \'size()\'" << std::endl;
    const size_t qty = tree_.size();
    //std::cout << "Intermediate result: " << qty << std::endl;
    const int int_qty = static_cast<int>( qty );
    //std::cout << "Final result: " << int_qty << std::endl;
    return int_qty;
}

void FrameSelector::computeMeanStd()
{
    mean_.resize( DIMS );
    inv_std_.resize( DIMS );
    for ( size_t i=0; i<DIMS; i++ )
    {
        mean_[i] = 0.0;
        inv_std_[i]  = 0.0;
    }
    const size_t qty = tree_.size();
    for ( size_t i=0; i<qty; i++ )
    {
        for ( size_t j=0; j<DIMS; j++ )
        {
            const Float v = tree_.val( i, j );
            mean_[j] += v;
        }
    }
    for ( size_t j=0; j<DIMS; j++ )
    {
        const Float m = mean_[j] / static_cast<Float>( qty );
        mean_[j] = m;
    }

    for ( size_t i=0; i<qty; i++ )
    {
        for ( size_t j=0; j<DIMS; j++ )
        {
            const Float v = tree_.val( i, j );
            const Float m = mean_[j];
            const Float dv = v - m;
            inv_std_[j] += dv*dv;
        }
    }
    for ( size_t j=0; j<DIMS; j++ )
    {
        const Float std = std::sqrt( inv_std_[j] / static_cast<Float>( qty ) );
        if ( std > EPS )
            inv_std_[j] = 1.0/std;
        else
            inv_std_[j] = 1.0;
    }
}

void FrameSelector::computeMeanAmp( double cutoff )
{
    mean_.resize( DIMS );
    inv_std_.resize( DIMS );
    std::array<double, DIMS> vmin, vmax;
    std::array<bool, DIMS> assigned;
    for ( size_t i=0; i<DIMS; i++ )
    {
        mean_[i]    = 0.0;
        inv_std_[i] = 0.0;
        assigned[i] = false;
    }
    const size_t qty = tree_.size();
    for ( size_t i=0; i<qty; i++ )
    {
        for ( size_t j=0; j<DIMS; j++ )
        {
            const Float v = tree_.val( i, j );
            mean_[j] += v;

            const bool ass = assigned[j];
            if ( !ass )
            {
                vmin[j] = vmax[j] = v;
                assigned[j] = true;
            }
            else
            {
                double & currMin = vmin[j];
                if ( v < currMin )
                    currMin = v;
                double & currMax = vmax[j];
                if ( v > currMax )
                    currMax = v;
            }
        }
    }
    for ( size_t j=0; j<DIMS; j++ )
    {
        const Float m = mean_[j] / static_cast<Float>( qty );
        mean_[j] = m;

        double & currMin = vmin[j];
        currMin = (currMin - m)*cutoff + m;

        double & currMax = vmax[j];
        currMax = (currMax - m)*cutoff + m;
    }

    for ( size_t j=0; j<DIMS; j++ )
    {
        const Float std = vmax[j] - vmin[j];
        if ( std > EPS )
            inv_std_[j] = 1.0/std;
        else
            inv_std_[j] = 1.0;
    }
}

void FrameSelector::normalize()
{
    const size_t qty = tree_.size();
    // Apply to all the entries.
    for ( size_t j=0; j<DIMS; j++ )
    {
        const Float mean    = mean_[j];
        const Float inv_std = inv_std_[j];
        for ( size_t i=0; i<qty; i++ )
        {
            const Float v = tree_.val(i, j);
            const Float adjustedV = (v-mean)*inv_std;
            tree_.val(i, j) = adjustedV;
        }
    }
}

void FrameSelector::overrideVal( size_t nodeInd, size_t valInd, FrameSelector::Float val )
{
    tree_.val(nodeInd, valInd) = val;
}

void FrameSelector::buildTree()
{
    tree_.build_tree();
}

const FrameSelector::Vector & FrameSelector::mean() const
{
    return mean_;
}

const FrameSelector::Vector & FrameSelector::invStd() const
{
    return inv_std_;
}

void FrameSelector::setMean( int ind, double v )
{
    mean_[ind] = v;
}

void FrameSelector::setInvStd( int ind, double v )
{
    inv_std_[ind] = v;
}

size_t FrameSelector::closestFrame( const std::vector<double> & frame )
{
    point<Float, DIMS, size_t> n( frame, 0 );

    const bool debug = tree_.debug_output();
    if ( debug )
    {
        std::cout << "distanceToNode() DIMS: " << DIMS << std::endl;
    }


    for ( size_t i=0; i<DIMS; i++ )
    {
        const Float v = n.get( i );
        const Float mean    = mean_[i];
        const Float inv_std = inv_std_[i];
        const Float normalizedV = (v - mean) * inv_std;
        n.get( i ) = normalizedV;
        if ( debug )
        {
            std::cout << "v: " << v << ", mean: " << mean 
                      << ", inv_std: " << inv_std << ", normalized: " << normalizedV 
                      << std::endl;
        }
    }
    const size_t ind = tree_.nearest_object( n );
    return ind;
}

double FrameSelector::closestDistance()
{
    const double res = tree_.distance();
    return res;
}

double FrameSelector::distanceToNode( const std::vector<double> & frame, size_t nodeInd )
{
    point<Float, DIMS, size_t> n( frame, 0 );

    const bool debug = tree_.debug_output();
    if ( debug )
    {
        std::cout << "distanceToNode() DIMS: " << DIMS << std::endl;
    }

    for ( size_t i=0; i<DIMS; i++ )
    {
        const Float v = n.get( i );
        const Float mean    = mean_[i];
        const Float inv_std = inv_std_[i];
        const Float normalizedV = (v - mean) * inv_std;
        n.get( i ) = normalizedV;
        if ( debug )
        {
            std::cout << "v: " << v << ", mean: " << mean 
                      << ", inv_std: " << inv_std << ", normalized: " << normalizedV 
                      << std::endl;
        }
    }
    const double d = tree_.distance_to_node( n, nodeInd );
    return d;
}

void FrameSelector::setDebug( bool en )
{
    tree_.set_debug_output( en );
}








