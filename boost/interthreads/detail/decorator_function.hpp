#ifndef BOOST_INTERTHREADS_DECORATION_FUNCTION__HPP
#define BOOST_INTERTHREADS_DECORATION_FUNCTION__HPP

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2008-2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or
// copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interthreads for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/interthreads/detail/config.hpp>
#include <boost/thread/detail/move.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/utility/result_of.hpp>

//#define BOOST_INTERTHREADS_THREAD_DECORATOR_MOVE

#include <boost/config/abi_prefix.hpp>

namespace boost {
namespace interthreads {
    namespace detail {
        template <typename T>
        struct decorator_function_base {
            virtual ~decorator_function_base() {}
            virtual T operator()() const=0;
        };

        template<typename F>
        struct BOOST_INTERTHREADS_DECL decorator_function : decorator_function_base<typename boost::result_of<F()>::type> {
            F f_;

            decorator_function(F f)
                : f_(f)
                {}

            typename boost::result_of<F()>::type operator()() const {
                return f_();
            }
        private:
            decorator_function();

        };

        template<typename F>
        static inline boost::shared_ptr<decorator_function_base<typename boost::result_of<F()>::type> > make_decorator_function(F f)
        {
            return boost::shared_ptr<decorator_function_base<typename boost::result_of<F()>::type> >(new decorator_function<F>(f));
        }
    }

}
}

#include <boost/config/abi_suffix.hpp>


#endif

