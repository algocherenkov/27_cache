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

BOOST_AUTO_TEST_CASE(cache_base_test)
{
    using namespace std::chrono_literals;

    TimeBasedCache<std::string, int> cache(10, 25);

    for(int i = 0; i < 11; i++)
        cache.set(i, "item_" + std::to_string(i));

    auto result = cache.get(1);
    BOOST_CHECK_MESSAGE(result.first == true, "wrong result, cache should have item_1");

    result = cache.get(0);
    BOOST_CHECK_MESSAGE(result.first == false, "wrong result, cache should have replace item_0 with item_1");

    std::this_thread::sleep_for(25ms);

    result = cache.get(10);
    BOOST_CHECK_MESSAGE(result.first == true, "wrong result, cache should have item_10 up to this moment");

    std::this_thread::sleep_for(100us);

    result = cache.get(1);
    BOOST_CHECK_MESSAGE(result.first == false, "wrong result, cache should have delete item_1 since time expired");
}

BOOST_AUTO_TEST_SUITE_END()
