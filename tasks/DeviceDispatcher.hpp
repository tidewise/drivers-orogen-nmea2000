#ifndef NMEA2000_DEVICE_DISPATCHER_HPP
#define NMEA2000_DEVICE_DISPATCHER_HPP

#include <nmea2000/nmea2000Types.hpp>
#include <nmea2000/Message.hpp>
#include <rtt/OutputPort.hpp>
#include <set>

namespace nmea2000 {
    /** Common implementation of the device filtering implementation
     * for both CANTask and ActisenseTask
     */
    class DeviceDispatcher {
    public:
        enum QueryState {
            QUERY_COMPLETE, //! all devices have been resolved
            QUERY_TIMED_OUT, //! not all devices have been resolved in the allocated time
            QUERY_IN_PROGRESS
        };

    private:
        struct DynamicPort {
            DeviceFilter filter;
            bool needs_resolution = false;
            uint8_t bus_id = 0;
            uint32_t pgn;

            RTT::OutputPort<Message>* port = nullptr;

            bool matches(Message const& msg) const;
        };

        std::vector<DynamicPort> m_dynamic_ports;

        base::Time m_resolution_timeout;
        base::Time m_enumeration_ack_timeout;
        bool m_continuous_query = false;

        bool m_needs_resolution = true;
        base::Time m_resolution_deadline;
        base::Time m_enumeration_ack_deadline;
        bool m_query_address_claim = false;
        std::set<int> m_pending_product_queries;

        std::vector<ResolvedDevice> processProductInformation(Message const& msg);

    public:
        DeviceDispatcher(
            std::vector<DeviceFilter> devices,
            base::Time const& resolution_timeout = base::Time::fromSeconds(25),
            base::Time const& enumeration_ack_timeout = base::Time::fromSeconds(5)
        );
        ~DeviceDispatcher();

        void setContinuousDeviceEnumeration(bool enable);

        bool getContinuousDeviceEnumeration() const;

        std::pair<bool, Message> getQueryProbeMessage();

        QueryState getQueryState() const;

        /** Return the bus ID of a resolved filter, or 0 if this filter
         * is not yet resolved
         */
        int getResolvedFilterID(std::string const& name) const;

        /** Return the names of the filters matching the given message
         */
        std::set<std::string> getMatchedFilters(Message const& msg) const;

        /** Reset the query timeout logic */
        void resetQueryLogic();

        /** Resolve the given product in the dynamic ports
         *
         * @param msg a ProductInformation message
         */
        std::vector<ResolvedDevice> process(Message const& msg);

        /** Dispatch a message on a corresponding port
         *
         * @return true if a matching port was found, false otherwise
         */
        bool dispatch(Message const& msg);

        void addPorts(RTT::TaskContext& task);
        void removePorts(RTT::TaskContext& task);
    };
}

#endif
