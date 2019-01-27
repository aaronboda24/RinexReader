#pragma once
/*
* Rinex3Obs.h
* Read and organize Rinex v3 observation file in epochwise manner
*  Created on: Jun 10, 2018
*  Updated on: January 26, 2019
*      Author: Aaron Boda
*/

#include "pch.h"
#include "TimeUtils.h"
#include "StringUtils.h"

#ifndef RINEX3OBS_H_
#define RINEX3OBS_H_

class Rinex3Obs
{
public:
	// CONSTRUCTOR
	Rinex3Obs();
	// DESTRUCTOR
	~Rinex3Obs();

	// DATA STRUCTURES
	// To store important header information
	struct ObsHeaderInfo {
		std::string rinexType;
		std::vector<double> approxPosXYZ;
		std::vector<double> antDeltaHEN;
		std::vector<double> firstObsTime;
		std::vector<double> lastObsTime;
		std::map<std::string, std::vector<std::string>> obsTypes;
    };
	// To store observations in an epoch
	struct ObsEpochInfo {
		std::vector<double> epochRecord;
		double recClockOffset;
		double gpsTime;
		int numSatsGPS; 
		int numSatsGLO; 
		int numSatsGAL;
		std::map<std::string, std::map<int, std::vector<double>>> observations;
		void clear() {
			epochRecord.clear();
			numSatsGAL = NULL;
			numSatsGLO = NULL;
			numSatsGPS = NULL;
			observations.clear();
			recClockOffset = NULL;
		}
    };

	// Attributes
	Rinex3Obs::ObsHeaderInfo _Header;
	Rinex3Obs::ObsEpochInfo _EpochObs;

	// * Available observation types (C1C, L1C,...)
	std::vector<std::string> _obsTypesGPS;
	std::vector<std::string> _obsTypesGLO;
	std::vector<std::string> _obsTypesGAL;

	// * Epoch observations mapped to PRN for ease of use
	std::map<int, std::vector<double>> _obsGPS;
	std::map<int, std::vector<double>> _obsGLO;
	std::map<int, std::vector<double>> _obsGAL;

	// Functions
	void obsHeader(std::ifstream& infile);
    void obsEpoch(std::ifstream& infile);
	void clear(Rinex3Obs::ObsEpochInfo& obs);
	void clear(Rinex3Obs::ObsHeaderInfo& header);
	void setObservations(std::map<std::string, std::map<int, std::vector<double>>> observations);
	std::map<int, double> specificObsMapper(std::map<int, std::vector<double>> obsGPS, std::vector<std::string> obsTypes, std::string specificObs);

private:

};

#endif /* RINEX3OBS_H_ */
