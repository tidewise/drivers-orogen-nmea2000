#ifndef nmea2000_TYPES_HPP
#define nmea2000_TYPES_HPP

#include <base/Angle.hpp>
#include <base/Float.hpp>
#include <base/Time.hpp>

namespace nmea2000 {
    struct DeviceFilter {
        std::string name;

        uint32_t pgn = 0;
        uint16_t product_code = 0;
        std::string model;
        std::string version;
        std::string serial_number;
    };

    struct ResolvedDevice {
        std::string name;
        uint8_t bus_id;
    };

    /** Measurement quality factor reported by Airmar depth sounders */
    struct AirmarAltitudeQuality {
        base::Time time;

        /** The airmar-specific factor
         *
         * Set to base::unknown if there is no bottom lock
         */
        float quality_factor;
    };

    /** Relative Humidity Measurement provided by the Yatch YDHS-01 sensor*/
    struct Humidity{
        base::Time time;

        /** Humidity level, from 0..1*/
        float value;
    };
}

#endif

