#pragma once
/*
* R2NavData.h
* Read and organize Rinex v2 navigation/ephemeris file 
*
*  Created on: Jun 12, 2018
*      Author: Aaron Boda
*/

#include "pch.h"
#include "StringUtils.h"
#include "TimeUtils.h"

#ifndef R2NAVDATA_H_
#define R2NAVDATA_H_

class R2NavData
{
public:
	// CONSTRUCTOR
	R2NavData();
	// DESTRUCTOR
	~R2NavData();

	// Data Structures
	struct HeaderGPS {
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
	HeaderGPS header;
	std::map<int, std::vector<R2NavData::DataGPS>> navData;

	// Functions
	void readNav(std::ifstream& inputfileGPS);

private:

};

#endif /* R3NAVDATA_H_ */
