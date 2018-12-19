// RinexReader.cpp : Shows how to use the Rinex (version 2) Observation and Navigation file reader.
//

#include "pch.h"
#include "FileIO.h"
#include "R2ObsData.h"
#include "R2NavData.h"

using namespace std;

// Epoch Time Matcher, returns index of most appropriate Navigation vector
int EpochMatcher(double obsTime, vector<R2NavData::DataGPS> NAV) {
	// Initialize time difference variable using arbitrary large number
	double diff = 1000000; int index = 0;
	for (unsigned i = 0; i < NAV.size(); i++) {
		double new_diff = fabs(obsTime - NAV.at(i).gpsTime);
		if (new_diff < diff) {
			diff = new_diff; // update difference
			index = i; // save index
		}
	}
	// Index of most appropriate Nav vector
	return index;
}

// Shows how to utilize the Rinex observation reader
int main()
{
	// *** INPUT RINEX FILE PATH
	string filePathObs = "Input/Obs.17o";
	string filePathNav = "Input/Nav.17n";
	string filePathLog = "LOG.txt";
	
	// *** CREATE FILE POINTER
	FileIO obsIN, navIN, logOUT;
	// Input File Streams
	ifstream fin_obs, fin_nav;
	ofstream fout_log;
	
	// Setting up the pointers, handling errors
	obsIN.fileSafeIn(filePathObs, fin_obs);
	navIN.fileSafeIn(filePathNav, fin_nav);
	logOUT.fileSafeOut(filePathLog, fout_log);

	// Prepare Log File
	logOUT.logger(filePathLog, filePathObs, fout_log);

	// *** NAVIGATION FILE DATA OBJECT
	R2NavData NAV;
	// Read and store ephemeris file
	NAV.readNav(fin_nav);
	// Navigation data storage variables
	vector<R2NavData::DataGPS> vecNav;
	R2NavData::DataGPS epochNav;

	// *** OBSERVATION FILE DATA OBJECT
	R2ObsData OBS;
	// Read Obs Header from input file
	OBS.obsHeader(fin_obs);

	// *** LOOP THROUGH OBSERVATION FILE IN EPOCHWISE MANNER
	while (!fin_obs.eof()) {
		// Read Epoch
		OBS.clearObs();
		OBS.obsEpoch(fin_obs, fout_log, OBS.header.nObsTypes);

		// Epoch Information
		cout << "OBS GPS Time : " << OBS.obsData.gpsTime << "\n";

		// Extract Observations
		map<int, double> obsC1 = OBS.specificObsMapper(OBS.obsGPS, OBS.obsTypesGPS, "C1"); // Code 
		map<int, double> obsL1 = OBS.specificObsMapper(OBS.obsGPS, OBS.obsTypesGPS, "L1"); // L1 Phase

		// *** FIND RELEVANT EPHEMERIS DATA
		map<int, double>::iterator itOBS = obsC1.begin();
		map<int, vector<R2NavData::DataGPS>>::iterator itNAV;

		// Iterate through PRN's to find corresponding ephemeris data
		for (itOBS = obsC1.begin(); itOBS != obsC1.end(); itOBS++) {
			// Current Satellite PRN
			int prn = itOBS->first;

			// Extract corresponding PRN Navigation data
			itNAV = NAV.navData.find(prn);
			if (itNAV != NAV.navData.end()) { vecNav = itNAV->second; }
			else { continue; }

			// Find most appropriate navigation data based on time comparison
			int idx = EpochMatcher(OBS.obsData.gpsTime, vecNav);
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

