#include <gtest/gtest.h>
#include "../tasks/DeviceDispatcher.hpp"
#include <nmea2000/PGNs.hpp>
#include <nmea2000/Receiver.hpp>

using namespace std;
using namespace nmea2000;

canbus::Message makeCAN(uint32_t can_id, std::vector<uint8_t> const& data) {
    canbus::Message can;
    can.can_id = can_id;
    can.size = data.size();
    copy(data.begin(), data.end(), can.data);
    return can;
}

struct DeviceDispatcherTest : public ::testing::Test {
    Message iso_address_claim;
    Message product_info;

    DeviceFilter getPGNFilter(int pgn) {
        DeviceFilter filter;
        filter.name = "pgn";
        filter.pgn = pgn;
        return filter;
    }

    DeviceFilter getSerialNumberFilter(string const& sn = "0002750") {
        DeviceFilter filter;
        filter.name = "serial_number";
        filter.serial_number = sn;
        return filter;
    }

    DeviceDispatcherTest() {
        // ISO Address message taken from logged data
        canbus::Message iso_address_claim_can = makeCAN(
            0x18EEFF90,
            { 0xBE, 0x0A, 0x80, 0x50, 0x00, 0x96, 0x96, 0xC0 }
        );
        iso_address_claim = Message::fromCAN(iso_address_claim_can);

        // Product info reply taken from log data
        vector<canbus::Message> product_info_can {
            makeCAN(0x19F01490, { 0x20, 0x86, 0xD0, 0x07, 0xEE, 0x3A, 0x4E, 0x53 }),
            makeCAN(0x19F01490, { 0x21, 0x35, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x25, 0x00, 0x00, 0x56, 0x31, 0x2E, 0x30, 0x31 }),
            makeCAN(0x19F01490, { 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56 }),
            makeCAN(0x19F01490, { 0x2A, 0x31, 0x2E, 0x30, 0x31, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x2D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x2E, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x32 }),
            makeCAN(0x19F01490, { 0x2F, 0x37, 0x35, 0x30, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }),
            makeCAN(0x19F01490, { 0x33, 0x01, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF })
        };
        auto library(pgns::getLibrary());
        Receiver receiver(library);
        for (auto can : product_info_can) {
            product_info = receiver.process(Message::fromCAN(can)).second;
        }
    }
};

TEST_F(DeviceDispatcherTest, it_throws_if_some_filters_have_no_names) {
    DeviceFilter filter;
    ASSERT_THROW(DeviceDispatcher({ filter }), invalid_argument);
}

TEST_F(DeviceDispatcherTest, it_starts_in_COMPLETE_state_if_there_are_no_filters) {
    DeviceDispatcher dispatcher({ });
    auto state = dispatcher.getQueryState();
    ASSERT_EQ(DeviceDispatcher::QUERY_COMPLETE, state);
    ASSERT_EQ((make_pair(false, Message())),
              dispatcher.getQueryProbeMessage());
}

TEST_F(DeviceDispatcherTest, it_starts_in_COMPLETE_state_if_all_filters_only_check_the_pgn) {
    DeviceDispatcher dispatcher({ getPGNFilter(42) });
    auto state = dispatcher.getQueryState();
    ASSERT_EQ(DeviceDispatcher::QUERY_COMPLETE, state);
    ASSERT_EQ((make_pair(false, Message())),
              dispatcher.getQueryProbeMessage());
}

TEST_F(DeviceDispatcherTest, it_starts_with_a_address_claim_query_if_a_filter_needs_product_info) {
    DeviceDispatcher dispatcher({ getSerialNumberFilter() });
    auto state = dispatcher.getQueryState();
    ASSERT_EQ(DeviceDispatcher::QUERY_IN_PROGRESS, state);
    ASSERT_EQ((make_pair(true, Receiver::queryAddressClaim())),
              dispatcher.getQueryProbeMessage());
}

TEST_F(DeviceDispatcherTest, it_transitions_to_product_info_query_after_the_query_ack_timeout) {
    DeviceDispatcher dispatcher(
        { getSerialNumberFilter() },
        base::Time::fromSeconds(25), base::Time::fromMilliseconds(10)
    );

    dispatcher.getQueryState();
    dispatcher.getQueryProbeMessage();
    dispatcher.process(iso_address_claim);

    ASSERT_EQ(DeviceDispatcher::QUERY_IN_PROGRESS, dispatcher.getQueryState());
    ASSERT_EQ((make_pair(false, Message())), dispatcher.getQueryProbeMessage());

    usleep(20000);
    ASSERT_EQ(DeviceDispatcher::QUERY_IN_PROGRESS, dispatcher.getQueryState());
    auto product_info = Receiver::queryProductInformation(iso_address_claim.source);
    ASSERT_EQ((make_pair(true, product_info)), dispatcher.getQueryProbeMessage());
}

TEST_F(DeviceDispatcherTest, it_transitions_to_COMPLETE_if_all_expected_product_info_are_received_and_all_filters_are_matched) {
    DeviceDispatcher dispatcher(
        { getSerialNumberFilter() },
        base::Time::fromSeconds(25), base::Time::fromMilliseconds(0)
    );

    dispatcher.getQueryState(); // switch to address claim query
    dispatcher.process(iso_address_claim);
    dispatcher.getQueryState(); // switch to product query
    dispatcher.process(product_info);
    auto state = dispatcher.getQueryState();
    ASSERT_EQ(DeviceDispatcher::QUERY_COMPLETE, state);
    ASSERT_EQ((make_pair(false, Message())),
              dispatcher.getQueryProbeMessage());
}

TEST_F(DeviceDispatcherTest, it_transitions_to_TIMED_OUT_if_all_expected_product_info_are_received_but_some_filters_are_not_matched) {
    DeviceDispatcher dispatcher(
        { getSerialNumberFilter("blabla") },
        base::Time::fromMilliseconds(20), base::Time::fromMilliseconds(0)
    );

    dispatcher.getQueryState(); // switch to address claim query
    dispatcher.process(iso_address_claim);
    dispatcher.getQueryState(); // switch to product query
    dispatcher.process(product_info);
    usleep(25000);
    auto state = dispatcher.getQueryState();
    ASSERT_EQ(DeviceDispatcher::QUERY_TIMED_OUT, state);
}

TEST_F(DeviceDispatcherTest, it_keeps_sending_address_claim_queries_if_the_product_info_times_out) {
    DeviceDispatcher dispatcher(
        { getSerialNumberFilter() },
        base::Time::fromSeconds(10),
        base::Time::fromMilliseconds(10)
    );

    // ISO Address message taken from logged data
    dispatcher.getQueryState();
    usleep(20000);
    auto state = dispatcher.getQueryState();
    ASSERT_EQ(DeviceDispatcher::QUERY_IN_PROGRESS, state);
    ASSERT_EQ((make_pair(true, Receiver::queryAddressClaim())),
              dispatcher.getQueryProbeMessage());
}

struct DeviceDispatcher_DispatchingTest : public DeviceDispatcherTest {
    DeviceDispatcher dispatcher;

    DeviceDispatcher_DispatchingTest()
        : dispatcher({ getSerialNumberFilter(), getPGNFilter(542) },
                     base::Time::fromSeconds(25), base::Time::fromMilliseconds(0)
          ) {

        dispatcher.getQueryState(); // switch to address claim query
        dispatcher.process(iso_address_claim);
        dispatcher.getQueryState(); // switch to product query
        dispatcher.process(product_info);
    }
};

TEST_F(DeviceDispatcher_DispatchingTest, it_associates_the_source_with_the_filter_in_case_product_information_is_used) {
    ASSERT_EQ(iso_address_claim.source, dispatcher.getResolvedFilterID("serial_number"));
}

TEST_F(DeviceDispatcher_DispatchingTest, it_resolves_a_PGN_message) {
    Message msg;
    msg.pgn = 542;
    ASSERT_EQ(set<string>{ "pgn" }, dispatcher.getMatchedFilters(msg));
}

TEST_F(DeviceDispatcher_DispatchingTest, it_works_with_aggregate_filters) {
    DeviceFilter filter;
    filter.name = "composite";
    filter.pgn = 542;
    filter.serial_number = "0002750";
    filter.model = "NS5";
    DeviceDispatcher dispatcher({ getSerialNumberFilter(), getPGNFilter(542), filter },
                                base::Time::fromSeconds(10), base::Time());
    dispatcher.getQueryState(); // switch to address claim query
    dispatcher.process(iso_address_claim);
    dispatcher.getQueryState(); // switch to product query
    dispatcher.process(product_info);

    ASSERT_EQ(iso_address_claim.source, dispatcher.getResolvedFilterID("composite"));

    Message msg;
    msg.source = iso_address_claim.source;
    msg.pgn = 542;

    ASSERT_EQ((set<string>{ "pgn", "serial_number", "composite" }),
              dispatcher.getMatchedFilters(msg));

    msg.source = 0;
    ASSERT_EQ(set<string>{ "pgn" }, dispatcher.getMatchedFilters(msg));

    msg.source = iso_address_claim.source;
    msg.pgn = 0;
    ASSERT_EQ(set<string>{ "serial_number" }, dispatcher.getMatchedFilters(msg));
}
