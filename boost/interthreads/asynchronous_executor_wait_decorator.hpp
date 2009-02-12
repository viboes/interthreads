#ifndef BOOST_INTERTHREADS_ASYNCHRONOUS_WAIT_EXECUTOR_DECORATOR__HPP
#define BOOST_INTERTHREADS_ASYNCHRONOUS_WAIT_EXECUTOR_DECORATOR__HPP

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

#include <boost/utility/result_of.hpp>


#include <boost/config/abi_prefix.hpp>

namespace boost {
namespace interthreads {


    template <typename AE, template <class> class Decorator>
    struct asynchronous_executor_wait_decorator : AE {

#if 0
        template <typename F>
        static typename boost::result_of<F()>::type waiter(bool started, condition_variable cond, F f) {
            typename boost::result_of<F()>::type res = f();
            started
        }
#endif
        template <typename T>
        struct handle {
            typedef typename AE::template handle<T>::type type;
        };

        template <typename F>
        typename AE::template handle< typename boost::result_of<F()>::type >::type
        fork( F fn ) {
            bool started;
            condition_variable cond;
            typename result_of::fork<AE, F>::type act= this->AE::fork(Decorator<typename boost::result_of<F()>::type>(fn));
            while (!started) cond.wait();
            return boost::move(act);
        }

        asynchronous_executor_wait_decorator() :AE() {};

        template <typename Nullary>
#ifdef BOOST_THREAD_HAS_THREAD_ATTR
        asynchronous_executor_wait_decorator(thread::native_handle_attr_type& attr, Nullary f)
        : AE(attr, f)
#else
        asynchronous_executor_wait_decorator(Nullary f)
        : AE(f)
#endif
        {}

    // move support
#ifdef BOOST_HAS_RVALUE_REFS
#else
    asynchronous_executor_wait_decorator(boost::detail::thread_move_t<asynchronous_executor_wait_decorator> x)
            : AE(boost::move(*(static_cast<AE*>(&(x.member))))) {}

    asynchronous_executor_wait_decorator& operator=(boost::detail::thread_move_t<asynchronous_executor_wait_decorator> x) {
        return move();
    }

    operator boost::detail::thread_move_t<asynchronous_executor_wait_decorator>() {
        return move();
    }

    boost::detail::thread_move_t<asynchronous_executor_wait_decorator> move() {
            return boost::detail::thread_move_t<asynchronous_executor_wait_decorator>(*this);
    }

#endif


    };


    template <typename AE, template <class> class Decorator>
    struct get_future<asynchronous_executor_wait_decorator<AE, Decorator> > {
        template <typename T>
        struct future_type {
            typedef typename AE::template get_future<AE>::type type;
        };
        template <typename T>
        typename future_type<T>::type&
        operator()(typename AE::template handle<T>::type & j)
        { return j.get_future(); }
    };

}
}

#include <boost/config/abi_suffix.hpp>


#endif
