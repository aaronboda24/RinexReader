#pragma once
/*
* Rinex2Obs.h
* Read and organize Rinex v2 file in epochwise manner
*  Created on: Jun 16, 2018
*  updated on: Jan 21, 2019
*      Author: Aaron Boda
*/

#include "pch.h"
#include "TimeUtils.h"
#include "StringUtils.h"

#ifndef RINEX2OBS_H_
#define RINEX2OBS_H_

class Rinex2Obs
{
public:
	// CONSTRUCTOR
	Rinex2Obs();
	// DESTRUCTOR
	~Rinex2Obs();

	// Data Structures
	// To store important header information
	struct ObsHeaderInfo {
		std::string rinexType;
		std::vector<double> approxPosXYZ;
		std::vector<double> antDeltaHEN;
		std::vector<double> firstObsTime;
		std::vector<double> lastObsTime;
		std::vector<std::string> obsTypes;
		int nObsTypes;
	}; 
	// To store observations in an epoch
	struct ObsEpochInfo {
		std::vector<double> epochRecord;
		double recClockOffset;
		double gpsTime;
		int nSats;
		std::vector<int> sats;
		std::map<int, std::vector<double>> observations;
		std::map<int, std::vector<int>> LLI; // LLI Flag
		std::map<int, std::vector<int>> SS; // Signal Strength
	}; 

	// Attributes
	ObsHeaderInfo _header;
	ObsEpochInfo _obsDataGPS;

	std::vector<std::string> _obsTypesGPS;
	std::map<int, std::vector<double>> _obsGPS;

	// Functions
	void clearObs();
	void clearHeader();
	void obsHeader(std::ifstream& infile);
	void obsEpoch(std::ifstream& infile, std::ofstream& logfile, int nObsTypes);
	std::map<int, double> Rinex2Obs::specificObsMapper(std::map<int, std::vector<double>> obsGPS, std::vector<std::string> obsTypes, std::string specificObs);

private:

};

#endif /* RINEX2OBS_H_ */
