#pragma once

/*
* Rinex2Nav.h
* Read and organize Rinex v2 navigation/ephemeris file 
* Created on: Jun 12, 2018
* Updated on: Jan 21, 2018
*      Author: Aaron Boda
*/

#include "pch.h"
#include "StringUtils.h"
#include "TimeUtils.h"

#ifndef RINEX2NAV_H_
#define RINEX2NAV_H_

class Rinex2Nav
{
public:
	// CONSTRUCTOR
	Rinex2Nav();
	// DESTRUCTOR
	~Rinex2Nav();

	// Data Structures
	struct NavHeaderGPS {
		// Ionospheric alpha and beta constants
		std::vector<double> ialpha;
		std::vector<double> ibeta;
		// Time System correction
		std::vector<double> dUTC;
		// Leap Seconds
		int leap;
	};

	// NOTE : Naming convention based on RINEX Manual
	// i.e. Switching between upper/lowercase is deliberate

	struct DataGPS {
		bool isAvailable;
		int PRN;
		std::vector<double> epochInfo;
		double gpsTime;
		double clockBias;
		double clockDrift;
		double clockDriftRate;
		double IODE;
		double Crs;
		double Delta_n;
		double Mo;
		double Cuc;
		double Eccentricity;
		double Cus;
		double Sqrt_a;
		double TOE;
		double Cic;
		double OMEGA;
		double CIS;
		double Io;
		double Crc;
		double Omega;
		double Omega_dot;
		double IDOT;
		double L2_codes_channel;
		double GPS_week;
		double L2_P_data_flag;
		double svAccuracy;
		double svHealth;
		double TGD;
		double IODC;
		double transmission_time;
		double fit_interval;
	}; 
	
	// Attributes
	NavHeaderGPS _header;
	std::map<int, std::vector<Rinex2Nav::DataGPS>> _navDataGPS;

	// Functions
	void readNav(std::ifstream& inputNavfileGPS);
	int EpochMatcher(double obsTime, std::vector<Rinex2Nav::DataGPS> NAV);

private:

};

#endif /* RINEX2NAV_H_ */
