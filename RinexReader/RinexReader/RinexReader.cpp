/*
* RinexReader.cpp : Shows how to use the Rinex file reader.
*  Updated on: January 26, 2019
*  Author: Aaron Boda
*/

#include "pch.h"
#include "FileIO.h"
#include "Rinex2Nav.h"
#include "Rinex2Obs.h"
#include "Rinex3Nav.h"
#include "Rinex3Obs.h"
#include "TimeUtils.h"

using namespace std;

// Helper function to read Rinex 2.x Files
void ReadRinex2(ifstream &fin_obs, ifstream &fin_nav, ofstream &fout_log);
// Helper function to read Rinex 3.x Files
void ReadRinex3(ifstream &fin_obs, ifstream &fin_nav, ofstream &fout_log);

// The program starts and ends inside main.
int main()
{
	// *** INPUT RINEX FILE PATH
	//string filePathObs = "Input/Rinex2/OBS.19o";
	//string filePathNav = "Input/Rinex2/NAV.19n";
	string filePathObs = "Input/Rinex3/OBS.rnx";
	string filePathNav = "Input/Rinex3/NAV.rnx";
	string filePathLog = "LOG.txt";
	
	// *** CREATE FILE POINTER
	FileIO FIO;
	// Input File Streams
	ifstream fin_obs, fin_nav;
	ofstream fout_log;
	
	// Setting up the pointers, handling errors
	FIO.fileSafeIn(filePathObs, fin_obs);
	FIO.fileSafeIn(filePathNav, fin_nav);
	FIO.fileSafeOut(filePathLog, fout_log);

	// Prepare Log File
	FIO.logger(filePathLog, filePathObs, fout_log);

	// Check Rinex File Version and Type
	int rinex_version = 0; 	int rinex_type = 0;
	FIO.checkRinexVersionType(rinex_version, rinex_type, fin_obs);

	// Direct to appropriate file reader
	if (rinex_version == 2) {
		// Rinex2 only supports GPS files
		if (rinex_type == 0) {
			ReadRinex2(fin_obs, fin_nav, fout_log);
		}
		else {
			cout << "ERROR: Rinex 2 File Reader only supports GPS files.\n";
		}
	}
	if ((rinex_version == 3)) {
		ReadRinex3(fin_obs, fin_nav, fout_log);
	}

	// EOF
	cout << "Exiting main function.\n";
}

// Helper function to read Rinex 2 Files
void ReadRinex2(ifstream &fin_obs, ifstream &fin_nav, ofstream &fout_log) {
	// *** NAVIGATION FILE DATA OBJECT
	Rinex2Nav NAV;
	// Read and store ephemeris file
	NAV.readNav(fin_nav);
	// Navigation data storage variables
	vector<Rinex2Nav::DataGPS> vecNav;
	Rinex2Nav::DataGPS epochNav;

	// *** OBSERVATION FILE DATA OBJECT
	Rinex2Obs OBS;
	// Read Obs Header from input file
	OBS.obsHeader(fin_obs);

	// *** LOOP THROUGH OBSERVATION FILE IN EPOCHWISE MANNER
	while (!(fin_obs >> std::ws).eof()) {
		// *** Deal with end of file error
		if (fin_obs.fail()) { break; }
		// *** 
		// Read Epoch
		OBS.clearObs();
		OBS.obsEpoch(fin_obs, fout_log, OBS._header.nObsTypes);

		// Epoch Information
		string obsTimeHMS = HHMMSS(OBS._obsDataGPS.epochRecord[3], OBS._obsDataGPS.epochRecord[4], OBS._obsDataGPS.epochRecord[5]);
		cout << "OBS Time : " << obsTimeHMS << "\n";

		// Extract Observations
		map<int, double> obsC1 = OBS.specificObsMapper(OBS._obsGPS, OBS._obsTypesGPS, "C1"); // Code 
		map<int, double> obsL1 = OBS.specificObsMapper(OBS._obsGPS, OBS._obsTypesGPS, "L1"); // L1 Phase

		// *** FIND RELEVANT EPHEMERIS DATA
		map<int, double>::iterator itOBS = obsC1.begin();
		map<int, vector<Rinex2Nav::DataGPS>>::iterator itNAV;

		// Iterate through PRN's to find corresponding ephemeris data
		for (itOBS = obsC1.begin(); itOBS != obsC1.end(); itOBS++) {
			// Current Satellite PRN
			int prn = itOBS->first;

			// Extract corresponding PRN Navigation data
			itNAV = NAV._navDataGPS.find(prn);
			if (itNAV != NAV._navDataGPS.end()) { vecNav = itNAV->second; }
			else { continue; }

			// Find most appropriate navigation data based on time comparison
			int idx = NAV.EpochMatcher(OBS._obsDataGPS.gpsTime, vecNav);
			epochNav = vecNav[idx];

			cout << "\t" << "PRN: " << epochNav.PRN << "\t\t" << "NAV GPS Time: " << epochNav.gpsTime << "\n";

			// *** DO REQUIRED PROCESSING HERE
			// 1 ) Compute Satellite Position
			// 2 ) Tropo / Iono Correction

		}

		// *** DO REQUIRED PROCESSING HERE
		// 1 ) Compute Receiver Position (Least Squares / KF)
		// 2 ) Output Solution to file 

		cout << endl;

	}

	cout << "Reached End of file..." << endl;
}

// Helper function to read Rinex 3 Files
void ReadRinex3(ifstream &fin_obs, ifstream &fin_nav, ofstream &fout_log) {
	// *** NAVIGATION FILE DATA OBJECT
	Rinex3Nav NAV;
	// Read and store ephemeris file
	NAV.readMixed(fin_nav);
	// Navigation data storage variables
	vector<Rinex3Nav::DataGPS> vecNavGPS;
	Rinex3Nav::DataGPS epochNavGPS;

	// *** OBSERVATION FILE DATA OBJECT
	Rinex3Obs OBS;
	// Read Obs Header from input file
	OBS.obsHeader(fin_obs);

	// *** LOOP THROUGH OBSERVATION FILE IN EPOCHWISE MANNER
	while (!(fin_obs >> std::ws).eof()) {
		// *** Deal with end of file error
		if (fin_obs.fail()) { break; }
		// *** 
		// Read Epoch
		//OBS._EpochObs.clear();
		OBS.obsEpoch(fin_obs);

		// Epoch Information
		string obsTimeHMS = HHMMSS(OBS._EpochObs.epochRecord[3], OBS._EpochObs.epochRecord[4], OBS._EpochObs.epochRecord[5]);
		cout << "OBS Time : " << obsTimeHMS << "\n";

		// Extract Observations
		map<int, double> obsC1 = OBS.specificObsMapper(OBS._obsGPS, OBS._obsTypesGPS, "C1C"); // Code 
		map<int, double> obsL1 = OBS.specificObsMapper(OBS._obsGPS, OBS._obsTypesGPS, "L1C"); // L1 Phase

		// *** FIND RELEVANT EPHEMERIS DATA
		map<int, double>::iterator itOBS = obsC1.begin();
		map<int, vector<Rinex3Nav::DataGPS>>::iterator itNAV;

		// Iterate through PRN's to find corresponding ephemeris data
		for (itOBS = obsC1.begin(); itOBS != obsC1.end(); itOBS++) {
			// Current Satellite PRN
			int prn = itOBS->first;

			// Extract corresponding PRN Navigation data
			itNAV = NAV._navGPS.find(prn);
			if (itNAV != NAV._navGPS.end()) { vecNavGPS = itNAV->second; }
			else { continue; }

			// Find most appropriate navigation data based on time comparison
			int idx = NAV.EpochMatcher(OBS._EpochObs.gpsTime, vecNavGPS);
			epochNavGPS = vecNavGPS[idx];

			cout << "\t" << "PRN: " << epochNavGPS.PRN << "\t\t" << "NAV GPS Time: " << epochNavGPS.gpsTime << "\n";

			// *** DO REQUIRED PROCESSING HERE
			// 1 ) Compute Satellite Position
			// 2 ) Tropo / Iono Correction

		}

		// *** DO REQUIRED PROCESSING HERE
		// 1 ) Compute Receiver Position (Least Squares / KF)
		// 2 ) Output Solution to file 

		cout << endl;

	}

	cout << "Reached End of file..." << endl;
}