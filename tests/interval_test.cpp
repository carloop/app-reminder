/* Run these tests on a laptop
 */
#include "catch.hpp"
#include "interval.h"

TEST_CASE("Interval(index)") {
  Interval interval(3);
  REQUIRE(interval.getIndex() == 3);
}

TEST_CASE("Interval::value") {
  Interval interval(0);
  interval.setValue(10);
  REQUIRE(interval.getValue() == 10);
  interval.addValue(5);
  REQUIRE(interval.getValue() == 15);
}

TEST_CASE("Interval::limit") {
  Interval interval(0);
  interval.setLimit(100);
  REQUIRE(interval.getLimit() == 100);
}

TEST_CASE("Interval::limitReached") {
  WHEN("limit is not 0") {
    WHEN("limit is not reached") {
      Interval interval(0);
      interval.setLimit(100);
      interval.setValue(99);
      REQUIRE(interval.limitReached() == false);
    }
    WHEN("limit is reached") {
      Interval interval(0);
      interval.setLimit(100);
      interval.setValue(101);
      REQUIRE(interval.limitReached() == true);
    }
  }
  WHEN("limit is 0") {
    Interval interval(0);
    interval.setValue(101);
    REQUIRE(interval.limitReached() == false);
  }
}
