#include <iostream>
#include <chrono>

#include "phys_string.hpp"
#include "phys_units.hpp"
#include "phys_chrono.hpp"
#include "phys_math.hpp"
#include "phys_angle.hpp"


using AngleInDegrees = units::PhysicalUnitAngle<int, std::ratio<1>>;
using AngleInMinutes = units::PhysicalUnitAngle<int, std::ratio<1, 60>>;

using Meter = units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
using MeterSquared = units::PhysicalUnit<int, std::ratio<1>, std::ratio<2>>;
using CentiMeter = units::PhysicalUnit<int, std::centi, std::ratio<1>>;

template <typename V, typename F>
using PhysTime = units::PhysicalUnit<V, F, std::ratio<0>, std::ratio<0>, std::ratio<1>>;

int main(void) {
	AngleInDegrees angle(60);
	AngleInMinutes angle_minutes(angle);

	Meter meter(4);
	CentiMeter centimeter(meter);
	MeterSquared meter_squared = meter * meter;

	PhysTime<int, std::ratio<1>> seconds(1);
	std::chrono::duration<int> ch_seconds = std::chrono::duration_cast(seconds);
	PhysTime<int, std::ratio<1>> other_seconds = units::unit_cast(ch_seconds);

	std::cout << "Angle in minutes: " << angle.value() << std::endl;
	std::cout << "Length in centimeters: " << centimeter.value() << std::endl;

	std::cout << "Centimeter: " << centimeter.value() << std::endl;
	std::cout << "Meter squared: " << meter_squared.value() << std::endl;

	std::cout << "Seconds: " << seconds.value() << ", seconds count: " << ch_seconds.count() << std::endl;
	std::cout << "Other seconds: " << other_seconds.value() << std::endl;

	return 0;
	
}
