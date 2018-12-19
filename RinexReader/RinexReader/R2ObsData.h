#pragma once
/*
* R2ObsData.h
* Read and organize Rinex v2 file in epochwise manner
*
*  Created on: Jun 16, 2018
*      Author: Aaron
*/

#include "pch.h"
#include "TimeUtils.h"
#include "StringUtils.h"

#ifndef R2OBSDATA_H_
#define R2OBSDATA_H_

class R2ObsData
{
public:
	// CONSTRUCTOR
	R2ObsData();
	// DESTRUCTOR
	~R2ObsData();

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
	ObsHeaderInfo header;
	ObsEpochInfo obsData;

	std::vector<std::string> obsTypesGPS;
	std::map<int, std::vector<double>> obsGPS;

	// Functions
	void clearObs();
	void clearHeader();
	void obsHeader(std::ifstream& infile);
	void obsEpoch(std::ifstream& infile, std::ofstream& logfile, int nObsTypes);
	std::map<int, double> R2ObsData::specificObsMapper(std::map<int, std::vector<double>> obsGPS, std::vector<std::string> obsTypes, std::string specificObs);

private:

};

#endif /* R2OBSDATA_H_ */
