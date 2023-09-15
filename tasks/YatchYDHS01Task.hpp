/* Generated from orogen/lib/orogen/templates/tasks/Task.hpp */

#ifndef NMEA2000_YATCHYDHS01TASK_TASK_HPP
#define NMEA2000_YATCHYDHS01TASK_TASK_HPP

#include "nmea2000/YatchYDHS01TaskBase.hpp"
#include <nmea2000/PGNs.hpp>

namespace nmea2000 {
    class YatchYDHS01Task : public YatchYDHS01TaskBase {
        friend class YatchYDHS01TaskBase;

    protected:
    public:
        YatchYDHS01Task(std::string const& name = "nmea2000::YatchYDHS01Task");

        ~YatchYDHS01Task();

        bool configureHook();
        bool startHook();

        void updateHook();
        void errorHook();
        void stopHook();
        void cleanupHook();
    private:
        uint8_t m_id;
        void updateHumidity(pgns::Humidity humidity_in);
        void updateTemperature(pgns::Temperature temperature_in);
    };
}

#endif
