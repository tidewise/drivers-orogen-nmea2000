/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "ActisenseTask.hpp"
#include <iodrivers_base/ConfigureGuard.hpp>
#include <nmea2000/ActisenseDriver.hpp>
#include "DeviceDispatcher.hpp"

using namespace std;
using namespace nmea2000;

ActisenseTask::ActisenseTask(std::string const& name)
    : ActisenseTaskBase(name)
{
    _resolution_timeout.set(base::Time::fromSeconds(20));
    _enumeration_ack_timeout.set(base::Time::fromSeconds(2));
}

ActisenseTask::~ActisenseTask()
{
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See ActisenseTask.hpp for more detailed
// documentation about them.

bool ActisenseTask::configureHook()
{
    // Un-configure the device driver if the configure fails.
    // You MUST call guard.commit() once the driver is fully
    // functional (usually before the configureHook's "return true;"
    unique_ptr<ActisenseDriver> driver(new ActisenseDriver());
    iodrivers_base::ConfigureGuard guard(this);
    if (!_io_port.get().empty()) {
        driver->openURI(_io_port.get());
    }
    setDriver(driver.get());

    // This is MANDATORY and MUST be called after the setDriver but before you do
    // anything with the driver
    if (! ActisenseTaskBase::configureHook()) {
        return false;
    }

    m_dispatcher.reset(new DeviceDispatcher(
        _devices.get(), _resolution_timeout.get(), _enumeration_ack_timeout.get()
    ));

    driver->sendStartupSequence();

    m_driver = move(driver);
    guard.commit();
    return true;
}

bool ActisenseTask::startHook()
{
    if (! ActisenseTaskBase::startHook()) {
        return false;
    }

    return true;
}
void ActisenseTask::updateHook()
{
    ActisenseTaskBase::updateHook();

    auto query_state = m_dispatcher->getQueryState();
    if (query_state == DeviceDispatcher::QUERY_TIMED_OUT &&
        state() != QUERY_TIMED_OUT) {
        state(QUERY_TIMED_OUT);
    }
    else if (query_state == DeviceDispatcher::QUERY_COMPLETE &&
             state() != QUERY_COMPLETE) {
        state(QUERY_COMPLETE);
    }

    auto query_message = m_dispatcher->getQueryProbeMessage();
    if (query_message.first) {
        m_driver->writeMessage(query_message.second);
    }
}

void ActisenseTask::processIO()
{
    try {
        auto msg = m_driver->readMessage();
        auto resolved_devices = m_dispatcher->process(msg);
        for (auto const& d : resolved_devices) {
            _resolved_devices.write(d);
        }
        _messages.write(msg);
    }
    catch (iodrivers_base::TimeoutError&) {
    }
}
void ActisenseTask::errorHook()
{
    ActisenseTaskBase::errorHook();
}
void ActisenseTask::stopHook()
{
    ActisenseTaskBase::stopHook();
}
void ActisenseTask::cleanupHook()
{
    m_dispatcher->removePorts(*this);
    ActisenseTaskBase::cleanupHook();
}
