//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2008-2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interthreads for documentation.
//
// Based on the shared.cpp example from the threadalert library of Roland Schwarz
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <algorithm>

#include <boost/progress.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/tp/pool.hpp>
#include <boost/tp/unbounded_channel.hpp>
#include <boost/tp/fifo.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/inplace_merge.hpp>
#include <boost/range/sub_range.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/array.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/adaptor/sliced.hpp>

#include <assert.h>


template <typename Range, std::size_t Parts>
class partition
{
public:
    boost::array<boost::sub_range<Range>,Parts> parts_;
    partition(boost::sub_range<Range>& range)
    {
        std::size_t size = boost::size(range);
        parts_[0]=boost::sub_range<Range>(boost::begin(range), boost::begin(range)+(size/Parts));
        for (std::size_t i=1; i< Parts-1; ++i) {
            parts_[i]=boost::sub_range<Range>(boost::begin(range)+i*(size/Parts), boost::begin(range)+(i+1)*(size/Parts));
        }
        parts_[Parts-1]=boost::sub_range<Range>(boost::begin(range)+(Parts-1)*(size/Parts), boost::end(range));
    }
    boost::sub_range<Range> operator[](unsigned i) {
        return parts_[i];
    }
};

typedef boost::tp::pool<
  boost::tp::unbounded_channel< boost::tp::fifo >
> pool_type;

#ifdef TASK_POOL
        typedef boost::tp::task< pool_type,  void > task_type;
#else
        typedef boost::tp::task< void > task_type;
#endif


template <
    typename DirectSolver,
    typename Composer,
    typename AE,
    typename Range
>
  void inplace_solve( AE & ae, boost::sub_range<Range> range, unsigned cutoff );
template <
    typename DirectSolver,
    typename Composer,
    typename AE,
    typename Range
>
  void inplace_solve( AE & ae, 
    boost::sub_range<Range> range, 
    unsigned cutoff)  {
    unsigned size = boost::size(range);
    //std::cout << "<<par_ " << size << std::endl;  
    if ( size <= cutoff) DirectSolver()(range);
    else {
#define BOOST_PARTS 2
        partition<Range, BOOST_PARTS> parts(range);
        task_type tasks[BOOST_PARTS];
        for (unsigned i=0;i < BOOST_PARTS-1; ++i) {
            task_type tmp(ae.submit(
                boost::bind(
                    &inplace_solve<DirectSolver,Composer,AE,Range>,
                    boost::ref(ae),
                    parts[i],
                    cutoff
            )));
            tasks[i] = tmp;

        }
        inplace_solve<DirectSolver,Composer,AE,Range>(ae, parts[BOOST_PARTS-1], cutoff);
        for (unsigned i=0;i < BOOST_PARTS-1; ++i) {
            tasks[i].wait();
        };
        
    //std::cout << "par_inplace_merge_fct " << size << ">>"<< std::endl;  
        Composer()(range);
    //std::cout << "par_ " << size << ">>"<< std::endl;  
        
    }
  }

struct sort_fct {
    template<class RandomAccessRange>
    RandomAccessRange& operator()(RandomAccessRange rng) {
        return boost::sort(rng);
    }
};

struct inplace_merge_fct {
    template<class BidirectionalRange>
    BidirectionalRange&
    operator()( BidirectionalRange rng) {
        return boost::inplace_merge(rng, boost::begin(rng)+(boost::size(rng)/2));
    }
};
template <typename Range>
void parallel_sort(Range& range, unsigned cutoff=10000) {
    pool_type pool( boost::tp::poolsize( 2) );
    boost::sub_range<Range> rng(boost::begin(range), boost::end(range));
    inplace_solve<sort_fct,inplace_merge_fct,pool_type,Range>( pool, rng, cutoff);
    //std::cout << "parallel_sort " << ">>"<< std::endl;  
}

#define NN 100000
int sorted[NN];
int values1[NN];
int values2[NN];
int values3[NN];
int values4[NN];
int values5[NN];
int values6[NN];

int main() {
    //pool_type ae(boost::tp::poolsize(2));
    for (unsigned i=0; i<NN; ++i) sorted[i]=i; 
   
    for (unsigned i=0; i<NN; ++i) values1[i]=NN-i-1;
    {
    std::cout << "std::sort: reverse 0.." << NN << std::endl;
    boost::progress_timer t;  // start timing
    std::sort(boost::begin(values1), boost::end(values1));
    }
    assert(boost::equal(values1, sorted));
    
    for (unsigned i=0; i<NN; ++i) values2[i]=NN-i-1;
    {
    std::cout << "boost::sort: reverse 0.."<<NN << std::endl;
    boost::progress_timer t;  // start timing
    boost::sort(values2);
    }
    assert(boost::equal(values2, sorted));

    // creates a threadpool with two worker-threads
    pool_type pool( boost::tp::poolsize( 2) );


    for (unsigned i=0; i<NN; ++i) values3[i]=NN-i-1;
    std::cout << "parallel_sort "<<NN/16<<":  reverse 0.."<<NN << std::endl;
    {
    boost::progress_timer tmr;  // start timing
    parallel_sort(values3, NN/16);
    }
    
    //for (unsigned i=0; i<NN; ++i) std::cout << sorted[i] << " " <<values3[i] << std::endl;
    assert(boost::equal(values3, sorted));

    for (unsigned i=0; i<NN; ++i) values4[i]=NN-i-1;
    {
    std::cout << "std::sort: reverse 0.." << NN << std::endl;
    boost::progress_timer t;  // start timing
    std::sort(boost::begin(values4), boost::end(values4));
    }
    assert(boost::equal(values4, sorted));
    
    for (unsigned i=0; i<NN; ++i) values5[i]=NN-i-1;
    std::cout << "parallel_sort "<<NN/16<<":  reverse 0.."<<NN << std::endl;
    {
    boost::progress_timer tmr;  // start timing
    parallel_sort(values5, NN/16);
    }
    
    //for (unsigned i=0; i<NN; ++i) std::cout << sorted[i] << " " <<values3[i] << std::endl;
    assert(boost::equal(values5, sorted));

    for (unsigned i=0; i<NN; ++i) values6[i]=NN-i-1;
    std::cout << "parallel_sort "<<NN/16<<":  reverse 0.."<<NN << std::endl;
    {
    boost::progress_timer tmr;  // start timing
    parallel_sort(values6, NN/16);
    }
    
    //for (unsigned i=0; i<NN; ++i) std::cout << sorted[i] << " " <<values3[i] << std::endl;
    assert(boost::equal(values6, sorted));

    //boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
    std::cout << "shutdown"<< std::endl;
    pool.shutdown();
    std::cout << "end"<< std::endl;
    return 0;
}
