/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "YatchYDHS01Task.hpp"
#include <nmea2000/PGNs.hpp>

using namespace nmea2000;

YatchYDHS01Task::YatchYDHS01Task(std::string const& name)
    : YatchYDHS01TaskBase(name)
{
}

YatchYDHS01Task::~YatchYDHS01Task()
{
}

void YatchYDHS01Task::updateHumidity(pgns::Humidity humidity_in)
{
    Humidity humidity_out;
    humidity_out.time = humidity_in.time;
    humidity_out.value = static_cast<int>(humidity_in.actual_humidity) * 0.004 / 100;
    _relative_humidity.write(humidity_out);
}

void YatchYDHS01Task::updateTemperature(pgns::Temperature temperature_in)
{
    base::samples::Temperature temperature_out;
    temperature_out = base::samples::Temperature::fromCelsius(temperature_in.time,
        temperature_in.actual_temperature);
    // Checks if this received temperature is the outside temperature
    // or if it's a dew point temperature and writes it on the
    // correct output port. According to the PGN:
    // 1 => OUTSIDE TEMPERATURE (DEFAULT)
    // 9 => DEW POINT TEMPERATURE
    if (temperature_in.temperature_source == 1) {
        _temperature.write(temperature_out);
    }
    else if (temperature_in.temperature_source == 9) {
        _dew_temperature.write(temperature_out);
    }
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See YatchYDHS01Task.hpp for more detailed
// documentation about them.

bool YatchYDHS01Task::configureHook()
{
    if (!YatchYDHS01TaskBase::configureHook())
        return false;
    m_id = _id.get();
    return true;
}
bool YatchYDHS01Task::startHook()
{
    if (!YatchYDHS01TaskBase::startHook())
        return false;
    return true;
}
void YatchYDHS01Task::updateHook()
{
    YatchYDHS01TaskBase::updateHook();

    Message msg;
    while (_msg_in.read(msg) == RTT::NewData) {
        if (msg.pgn == pgns::Humidity::ID) {
            pgns::Humidity in = pgns::Humidity::fromMessage(msg);
            if (in.humidity_instance == m_id) {
                updateHumidity(in);
            }
        }
        else if (msg.pgn == pgns::Temperature::ID) {
            pgns::Temperature in = pgns::Temperature::fromMessage(msg);
            if (in.temperature_instance == m_id) {
                updateTemperature(in);
            }
        }
    }
}

void YatchYDHS01Task::errorHook()
{
    YatchYDHS01TaskBase::errorHook();
}
void YatchYDHS01Task::stopHook()
{
    YatchYDHS01TaskBase::stopHook();
}
void YatchYDHS01Task::cleanupHook()
{
    YatchYDHS01TaskBase::cleanupHook();
}
