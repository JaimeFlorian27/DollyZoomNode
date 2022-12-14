#pragma once
#include <math.h>
class DollyZoom
{
public:
	static double milimetetersToinches(const double &focalLength);
	static double focalLengthToAngleOfView(const double &focalLength, const double &cameraAperture);
	static double angleOfViewToFocalLength(const double &angleOfView, const double &cameraAperture);
	static double calculateAOV(const double &width, const double &distance);
	static const double PI;
};
