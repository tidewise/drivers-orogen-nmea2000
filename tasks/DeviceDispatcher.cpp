#include "DeviceDispatcher.hpp"
#include <nmea2000/Receiver.hpp>
#include <rtt/TaskContext.hpp>

using namespace std;
using namespace nmea2000;

DeviceDispatcher::DeviceDispatcher(
    vector<DeviceFilter> device_filters,
    base::Time const& resolution_timeout,
    base::Time const& enumeration_ack_timeout
)
    : m_resolution_timeout(resolution_timeout)
    , m_enumeration_ack_timeout(enumeration_ack_timeout) {

    bool needs_resolution = false;
    for (auto const& filter : device_filters) {
        if (filter.name.empty()) {
            throw std::invalid_argument("found filter with no name");
        }

        DynamicPort port;
        port.filter = filter;
        port.pgn = filter.pgn;
        port.port = nullptr;

        port.needs_resolution = filter.product_code || !(
            filter.model.empty() &&
            filter.version.empty() &&
            filter.serial_number.empty()
        );
        needs_resolution = needs_resolution || port.needs_resolution;
        m_dynamic_ports.push_back(port);
    }
    m_needs_resolution = needs_resolution;
    resetQueryLogic();
}

DeviceDispatcher::~DeviceDispatcher() {
    for (auto const& p : m_dynamic_ports) {
        delete p.port;
    }
}

void DeviceDispatcher::addPorts(RTT::TaskContext& task) {
    for (auto const& p : m_dynamic_ports) {
        if (p.port) {
            throw invalid_argument(
                "addPort: attempting to create new ports, but the existing ones "\
                "have not yet been detached. Call #removePorts first"
            );
        }
        else if (task.getPort(p.filter.name)) {
            throw invalid_argument(
                "addPort: attempting to create a port named '" + p.filter.name +
                "' but it already exists on the interface of " + task.getName()
            );
        }
    }

    for (auto& p : m_dynamic_ports) {
        p.port = dynamic_cast<decltype(p.port)>(task.getPort(p.filter.name));
        if (!p.port) {
            p.port = new RTT::OutputPort<Message>(p.filter.name);
            task.provides()->addPort(*p.port);
        }
    }
}

void DeviceDispatcher::removePorts(RTT::TaskContext& task) {
    for (auto& p : m_dynamic_ports) {
        // Names can be shared across filters, make sure we haven't removed the
        // port already
        if (task.getPort(p.filter.name)) {
            task.provides()->removePort(p.filter.name);
            delete p.port;
        }
        p.port = nullptr;
    }
}

bool DeviceDispatcher::DynamicPort::matches(Message const& msg) const {
    return !needs_resolution &&
           (!pgn || msg.pgn == pgn) &&
           (!bus_id || msg.source == bus_id);
}


void DeviceDispatcher::setContinuousDeviceEnumeration(bool enable) {
    m_continuous_query = enable;
}

bool DeviceDispatcher::getContinuousDeviceEnumeration() const {
    return m_continuous_query;
}

std::pair<bool, Message> DeviceDispatcher::getQueryProbeMessage() {
    auto now = base::Time::now();

    if (now < m_enumeration_ack_deadline) {
        if (!m_pending_product_queries.empty() && m_received_expected_product_info) {
            m_received_expected_product_info = false;
            int query = *m_pending_product_queries.begin();
            return make_pair(true, Receiver::queryProductInformation(query));
        }
        return make_pair(false, Message());
    }
    else if (!m_needs_resolution && !m_continuous_query) {
        return make_pair(false, Message());
    }

    m_received_expected_product_info = false;
    m_query_address_claim = !m_query_address_claim;
    m_enumeration_ack_deadline = now + m_enumeration_ack_timeout;
    if (m_query_address_claim || m_pending_product_queries.empty()) {
        m_query_address_claim = true;
        m_pending_product_queries.clear();
        return make_pair(true, Receiver::queryAddressClaim());
    }
    else {
        int query = *m_pending_product_queries.begin();
        return make_pair(true, Receiver::queryProductInformation(query));
    }
}

DeviceDispatcher::QueryState DeviceDispatcher::getQueryState() const {
    if (!m_needs_resolution) {
        return QUERY_COMPLETE;
    }
    else if (base::Time::now() > m_resolution_deadline) {
        return QUERY_TIMED_OUT;
    }
    return QUERY_IN_PROGRESS;
}

vector<ResolvedDevice> DeviceDispatcher::process(Message const& msg) {
    if (msg.pgn == pgns::ISOAddressClaim::ID) {
        m_pending_product_queries.insert(msg.source);
        return vector<ResolvedDevice>();
    }
    else if (msg.pgn == pgns::ProductInformation::ID) {
        m_received_expected_product_info =
            (!m_pending_product_queries.empty() &&
             *m_pending_product_queries.begin() == msg.source);
        m_pending_product_queries.erase(msg.source);
        return processProductInformation(msg);
    }
    else {
        dispatch(msg);
        return vector<ResolvedDevice>();
    }
}

vector<ResolvedDevice> DeviceDispatcher::processProductInformation(Message const& msg) {
    vector<ResolvedDevice> resolved_devices;
    bool needs_resolution = false;
    auto info = pgns::ProductInformation::fromMessage(msg);
    for (auto& port : m_dynamic_ports) {
        if (!port.needs_resolution) {
            continue;
        }

        auto f = port.filter;
        bool matches = (
            (!f.product_code || f.product_code == info.product_code) &&
            (f.model.empty() || f.model == info.model_id) &&
            (f.version.empty() || f.version == info.model_version) &&
            (f.serial_number.empty() || f.serial_number == info.model_serial_code)
        );

        if (matches) {
            port.needs_resolution = false;
            port.bus_id = msg.source;

            ResolvedDevice resolved;
            resolved.name = f.name;
            resolved.bus_id = msg.source;
            resolved_devices.push_back(resolved);
        }
        else {
            needs_resolution = true;
        }
    }
    m_needs_resolution = needs_resolution;
    return resolved_devices;
}

bool DeviceDispatcher::dispatch(Message const& msg) {
    bool written = false;
    for (auto& port : m_dynamic_ports) {
        if (port.matches(msg)) {
            written = true;
            if (!port.port) {
                throw std::runtime_error(
                    "DeviceDispatcher: #dispatch called before #addPort"
                );
            }
            port.port->write(msg);
        }
    }
    return written;
}

void DeviceDispatcher::resetQueryLogic() {
    auto now = base::Time::now();
    // m_query_address_claim is the "current state", and will be flipped on the
    // first call to getQueryState. Initialize to false so that the first thing
    // we do is indeed an ISOAddressClaim query
    m_query_address_claim = false;
    m_pending_product_queries.clear();
    m_resolution_deadline = now + m_resolution_timeout;
    m_enumeration_ack_deadline = base::Time();
    m_received_expected_product_info = false;
}

set<string> DeviceDispatcher::getMatchedFilters(Message const& msg) const {
    set<string> result;
    for (auto& port : m_dynamic_ports) {
        if (port.matches(msg)) {
            result.insert(port.filter.name);
        }
    }
    return result;
}

int DeviceDispatcher::getResolvedFilterID(std::string const& name) const {
    for (auto& port : m_dynamic_ports) {
        if (port.filter.name == name) {
            return port.bus_id;
        }
    }
    throw std::invalid_argument("no filter with name '" + name + "'");
}
