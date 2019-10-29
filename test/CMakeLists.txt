rock_gtest(suite suite.cpp
           test_DeviceDispatcher.cpp
           DEPS nmea2000-tasks-${OROCOS_TARGET}
           DEPS_PKGCONFIG nmea2000)