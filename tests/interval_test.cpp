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

//TEST_CASE("OBDMessage::id") {
//  OBDMessage sut;
//  CANMessage msg;
//  msg.id = 0x7E8;
//  sut.addMessageData(msg);
//  REQUIRE(sut.id() == 0x7E8);
//}
//
//TEST_CASE("OBDMessage::addMessageData") {
//  OBDMessage sut;
//  CANMessage msg;
//  msg.len = 8;
//
//  SECTION("1 byte of data") {
//    msg.data[0] = 0x01;
//    msg.data[1] = 0x11;
//
//    bool needsFlowControl = sut.addMessageData(msg);
//    REQUIRE(needsFlowControl == false);
//    REQUIRE(sut.size() == 1);
//    REQUIRE(sut.complete() == true);
//    OBDMessage::DataT expected{ 0x11 };
//    REQUIRE(sut.data() == expected);
//  }
//
//  SECTION("3 byte of data") {
//    msg.data[0] = 0x03;
//    msg.data[1] = 0x11;
//    msg.data[2] = 0x22;
//    msg.data[3] = 0x33;
//
//    bool needsFlowControl = sut.addMessageData(msg);
//    REQUIRE(needsFlowControl == false);
//    REQUIRE(sut.size() == 3);
//    REQUIRE(sut.complete() == true);
//    OBDMessage::DataT expected{ 0x11, 0x22, 0x33 };
//    REQUIRE(sut.data() == expected);
//  }
//
//  SECTION("7 byte of data") {
//    msg.data[0] = 0x07;
//    msg.data[1] = 0x11;
//    msg.data[2] = 0x22;
//    msg.data[3] = 0x33;
//    msg.data[4] = 0x44;
//    msg.data[5] = 0x55;
//    msg.data[6] = 0x66;
//    msg.data[7] = 0x77;
//
//    bool needsFlowControl = sut.addMessageData(msg);
//    REQUIRE(needsFlowControl == false);
//    REQUIRE(sut.size() == 7);
//    REQUIRE(sut.complete() == true);
//    OBDMessage::DataT expected{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
//    REQUIRE(sut.data() == expected);
//  }
//
//  SECTION("8 byte of data") {
//    msg.data[0] = 0x10;
//    msg.data[1] = 0x08;
//    msg.data[2] = 0x11;
//    msg.data[3] = 0x22;
//    msg.data[4] = 0x33;
//    msg.data[5] = 0x44;
//    msg.data[6] = 0x55;
//    msg.data[7] = 0x66;
//
//    bool needsFlowControl = sut.addMessageData(msg);
//    REQUIRE(needsFlowControl == true);
//    REQUIRE(sut.size() == 8);
//    REQUIRE(sut.complete() == false);
//
//    msg.data[0] = 0x20;
//    msg.data[1] = 0x77;
//    msg.data[2] = 0x88;
//
//    needsFlowControl = sut.addMessageData(msg);
//    REQUIRE(needsFlowControl == false);
//    REQUIRE(sut.size() == 8);
//    REQUIRE(sut.complete() == true);
//
//    OBDMessage::DataT expected{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
//    REQUIRE(sut.data() == expected);
//  }
//}
//
//TEST_CASE("OBDMessage::flowControlMessage") {
//  OBDMessage sut;
//  CANMessage msg;
//  msg.id = 0x7E8;
//  sut.addMessageData(msg);
//
//  REQUIRE(sut.flowControlMessage() == CANMessage(0x7E0, { 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }));
//}
