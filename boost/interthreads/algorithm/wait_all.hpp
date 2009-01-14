//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2008-20009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interthreads for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERTHREADS_WAIT_ALL__HPP
#define BOOST_INTERTHREADS_WAIT_ALL__HPP

#include <boost/fusion/include/for_each.hpp>

#ifdef BOOST_HAS_CHRONO_LIB
#include <boost/chono/chono.hpp>
#else
#include <boost/thread/thread_time.hpp>
#endif

#include <boost/config/abi_prefix.hpp>

namespace boost {
namespace interthreads {

    namespace fct {
        struct wait {
            typedef void result_type;
            template<typename ACT>
            void operator()(ACT& act) const {
                //std::cout << "wait()" << std::endl;
                act.wait();
            }
        };
    }

    namespace result_of {
        template <typename Sequence>
        struct wait_all {
            typedef typename fusion::result_of::for_each<Sequence, fct::wait>::type type;
        };
    }

    template <typename Sequence>
    typename result_of::wait_all<Sequence>::type
    wait_all(Sequence& t) {
        return fusion::for_each(t, fct::wait());
    }

}    
}   // namespace boost

#include <boost/config/abi_suffix.hpp>

#endif