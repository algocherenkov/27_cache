#include "cache.h"
#include <string>
#include <ctime>
#include <chrono>
#include <random>
#include <iterator>
#include <map>

#define BOOST_TEST_MODULE test_main

#include <boost/test/included/unit_test.hpp>

using namespace boost::unit_test;
BOOST_AUTO_TEST_SUITE(test_suite_main)

BOOST_AUTO_TEST_CASE(cache_test)
{
    auto start = std::chrono::high_resolution_clock::now();

    // unsync the I/O of C and C++.
    std::ios_base::sync_with_stdio(false);



    auto end = std::chrono::high_resolution_clock::now();

    // Calculating total time taken by the BM algorithm.
    double time_taken =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9;
    std::cout << "BM algorithm's execution time: " << std::fixed << time_taken << std::setprecision(9) << " sec" << std::endl;
    std::ios_base::sync_with_stdio(true);
    //=============================================================================================
    start = std::chrono::high_resolution_clock::now();

    // unsync the I/O of C and C++.
    std::ios_base::sync_with_stdio(false);



    end = std::chrono::high_resolution_clock::now();

    // Calculating total time taken by the BM algorithm.
    time_taken =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9;
    std::cout << "KMP algorithm's execution time: " << std::fixed << time_taken << std::setprecision(9) << " sec" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
