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

    struct FluidLevel {
        base::Time time;

        float level = base::unknown<float>();
        float capacity = base::unknown<float>();
    };

    enum NavigationalStatus {
        STATUS_UNDER_WAY_USING_ENGINE = 0,
        STATUS_AT_ANCHOR = 1,
        STATUS_NOT_UNDER_COMMAND = 2,
        STATUS_RESTRICTED_MANOEUVERABILITY = 3,
        STATUS_RESTRICTED_BY_DRAUGHT = 4,
        STATUS_MOORED = 5,
        STATUS_AGROUND = 6,
        STATUS_ENGAGED_IN_FISHING = 7,
        STATUS_UNDER_WAY_SAILING = 8,
        STATUS_RESERVED0 = 9,
        STATUS_RESERVED1 = 10,
        STATUS_RESERVED2 = 11,
        STATUS_RESERVED3 = 12,
        STATUS_RESERVED4 = 13,
        STATUS_AIS_SART_ACTIVE = 14,
        STATUS_NOT_DEFINED = 15
    };

    /** Representation of the data stored in AIS Message 1 to 3 */
    struct AISPosition {
        base::Time time;

        /** The vessel MMSI number */
        int mmsi = 0;

        /** Vessel course over ground */
        base::Angle course_over_ground;
        base::Angle latitude;
        base::Angle longitude;
        NavigationalStatus status;

        /** Whether the GPS position is using differential (< 10m resolution) or
         * not
         */
        bool high_accuracy_position = false;
        /** Vessel heading */
        base::Angle yaw;
        /** Velocity in yaw, in rad/s */
        float yaw_velocity = base::unknown<float>();
        /** Speed over ground, in m/s */
        float speed_over_ground = base::unknown<float>();
    };

    /** Representation of the data stored in AIS Message 5 */
    struct AISVesselInformation {
        base::Time time;

        int mmsi = 0;
        int imo = 0;
        std::string call_sign;
        std::string name;
        int ship_type = 0;
        int position_device_type = 0;
        float draught = base::unknown<float>();
        float length = base::unknown<float>();
        float width = base::unknown<float>();
        float gross_weight = base::unknown<float>();
        float dead_weight = base::unknown<float>();

        base::Vector3d reference_position;
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
}

#endif

