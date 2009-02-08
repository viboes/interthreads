//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2008-2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or
// copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Based on the unique_threader/unique_joiner design from of Kevlin Henney (n1883)
//
// See http://www.boost.org/libs/interthreads for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERTHREADS_TYPEOF_BASIC_THREADER__HPP
#define BOOST_INTERTHREADS_TYPEOF_BASIC_THREADER__HPP

#include <boost/interthreads/basic_threader.hpp>
#include <boost/typeof/typeof.hpp>

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

BOOST_TYPEOF_REGISTER_TYPE(boost::interthreads::basic_threader)
BOOST_TYPEOF_REGISTER_TYPE(boost::thread)

#endif
