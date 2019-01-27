/*
* TimeUtils.cpp
* Functions used for time system conversions
*  Created on: Jun 10, 2018
*      Author: Aaron Boda
*/

#include "pch.h"
#include "TimeUtils.h"

using namespace std;

// Source: BOOK called GPS Theory Algorithm & Applications by Guochang Xu (Pg 18-20)
double gpsTime(std::vector<double> epochInfo) {
	// As precaution, check if we have required epoch info
	if (epochInfo.size() < 6) {
		// If so, we dont have enough epoch info to work with
		// We need to exit prematurely
		exit(1);
	}
	// Year Month Day of epoch
	double y = epochInfo.at(0); double m = epochInfo.at(1); double d = epochInfo.at(2);
	// Hour Minute Second of epoch
	double hr = epochInfo.at(3); double min = epochInfo.at(4); double sec = epochInfo.at(5);
	// UTC time in hours
	double UTC = hr + min / 60. + sec / 3600.;
	// Taking care of month and year conditioning
	if (m > 2) {
		y = y + 2000;
	}
	else {
		y = y + 2000 - 1;
		m = m + 12;
	}
	// Julian Date
	double jDate = floor(365.25*y) + floor(30.6001*(m + 1)) + d + (UTC / 24) + 1720981.5;
	// GPS Week
	double gpsWeek = floor((jDate - 2444244.5) / 7);
	// GPS Time
	double gpsTime = round(((((jDate - 2444244.5) / 7) - gpsWeek) * 7 * 24 * 3600) * 100) / 100;
	// Return GPS Time in seconds
	return gpsTime;
}

