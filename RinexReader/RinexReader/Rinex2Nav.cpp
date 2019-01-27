/*
* Rinex2Nav.h
* Read and organize Rinex v2 navigation/ephemeris file
*  Created on: Jun 12, 2018
*      Author: Aaron Boda
*/

#include "pch.h"
#include "Rinex2Nav.h"

using namespace std;

// CONSTRUCTOR AND DESTRUCTOR DEFINITIONS
Rinex2Nav::Rinex2Nav() {}
Rinex2Nav::~Rinex2Nav() {}

// A function to help with organizing GPS header
// Works for the alpha/beta ionospheric constants and time correction
vector<double> headerHelper(string line) {
	line = line.substr(2, 58);
	string word;
	vector<double> data;
	istringstream iss(line);
	vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
	for (string s : words) {
		data.push_back(stod(s));
	}
	return data;
}


// Organizes Epoch Time Information into Vector
vector<double> rinex2EpochTimeOrganizer(string line) {
	vector<double> epochRecord;
	line = line.substr(2, 20);
	// Splitting words in the line
	istringstream iss(line);
	vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
	for (string s : words) {
		epochRecord.push_back(stod(s));
	}
	// Return: Vector containing the Epoch Info
	return epochRecord;
}

// Function to split and organize navigation parameters
vector<double> rinex2NavDataSplitter(string line) {
	// Split line every 19 spaces as allocated for parameters
	vector<double> data;
	string word;
	for (unsigned i = 0; i < line.length(); i += 19) {
		word = line.substr(i, 19);
		if (word.find_first_not_of(' ') == std::string::npos) { continue; }
		data.push_back(stod(replaceChars(word, 'D', 'e')));
		word.clear();
	}
	return data;
}

// Epoch Time Matcher, returns index of most appropriate Navigation vector
int Rinex2Nav::EpochMatcher(double obsTime, std::vector<Rinex2Nav::DataGPS> NAV) {
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

// Navigation Body Organizer for GPS Navigation File
Rinex2Nav::DataGPS epochNavOrganizer(vector<string> block) {
	int prn = stoi(block[0].substr(0, 2));
	vector<double> epochInfo = rinex2EpochTimeOrganizer(block[0]);
	string line = block[0].substr(22, block[0].length());
	for (int i = 1; i < (int)block.size(); i++) {
		line = line + block[i];
	}
	vector<double> parameters = rinex2NavDataSplitter(line);
	// Storing Values into GPS Data Structure
	Rinex2Nav::DataGPS GPS;
	GPS.isAvailable = true;
	GPS.PRN = prn;
	GPS.epochInfo = epochInfo;
	GPS.gpsTime = gpsTime(epochInfo);
	GPS.clockBias = parameters.at(0);
	GPS.clockDrift = parameters.at(1);
	GPS.clockDriftRate = parameters.at(2);
	GPS.IODE = parameters.at(3);
	GPS.Crs = parameters.at(4);
	GPS.Delta_n = parameters.at(5);
	GPS.Mo = parameters.at(6);
	GPS.Cuc = parameters.at(7);
	GPS.Eccentricity = parameters.at(8);
	GPS.Cus = parameters.at(9);
	GPS.Sqrt_a = parameters.at(10);
	GPS.TOE = parameters.at(11);
	GPS.Cic = parameters.at(12);
	GPS.OMEGA = parameters.at(13);
	GPS.CIS = parameters.at(14);
	GPS.Io = parameters.at(15);
	GPS.Crc = parameters.at(16);
	GPS.Omega = parameters.at(17);
	GPS.Omega_dot = parameters.at(18);
	GPS.IDOT = parameters.at(19);
	GPS.L2_codes_channel = parameters.at(20);
	GPS.GPS_week = parameters.at(21);
	GPS.L2_P_data_flag = parameters.at(22);
	GPS.svAccuracy = parameters.at(23);
	GPS.svHealth = parameters.at(24);
	GPS.TGD = parameters.at(25);
	GPS.IODC = parameters.at(26);
	if (parameters.size() > 27) { GPS.transmission_time = parameters.at(27); }
	if (parameters.size() > 28) { GPS.fit_interval = parameters.at(28); }
	return GPS;
}

// Reader for GPS navigation file
void Rinex2Nav::readNav(std::ifstream& infile) {
	// String tokens to look for
	const string sTokenIALPHA = "ION ALPHA";
	const string sTokenIBETA = "ION BETA";
	const string sTokenUTC = "DELTA-UTC: A0,A1,T,W";
	const string sTokenLEAP = "LEAP SECONDS";
	const string sTokenEND = "END OF HEADER";
	const string sTokenCOM = "COMMENT";
	// A vector to hold block of sentences
	vector<string> block;
	// To hold contents of a line from input file
	string line;
	int nlines = 0;

	// Reading Header 
	while (!infile.eof()) {
		line.clear();
		// Temporarily store line from input file
		getline(infile, line, '\n');
		// Looking for keywords in Header Part...
		size_t found_ALPHA = line.find(sTokenIALPHA);
		size_t found_BETA = line.find(sTokenIBETA);
		size_t found_END = line.find(sTokenEND);
		size_t found_UTC = line.find(sTokenUTC);
		size_t found_LEAP = line.find(sTokenLEAP);
		size_t found_COM = line.find(sTokenCOM);

		// Finding Comments, meaning skip!
		if (found_COM != string::npos) {
			continue;
		}
		// Finding Ionophseric Constants as per new format
		else if (found_ALPHA != string::npos) {
			vector<double> ialpha = headerHelper(line);
			_header.ialpha = ialpha;
		}
		else if (found_BETA != string::npos) {
			vector<double> ibeta = headerHelper(line);
			_header.ibeta = ibeta;
		}
		// Finding GPS to UTC Time Correction
		else if (found_UTC != string::npos) {
			line = line.substr(1, 59);
			vector<double> dUTC;
			double A0 = stod(line.substr(0, 21)); dUTC.push_back(A0);
			double A1 = stod(line.substr(21, 21)); dUTC.push_back(A1);
			double tUTC = stod(line.substr(42, 9)); dUTC.push_back(tUTC);
			double week = stod(line.substr(51, line.length())); dUTC.push_back(week);
			_header.dUTC = dUTC;
		}
		// Finding Leap Seconds
		else if (found_LEAP != string::npos) {
			_header.leap = stoi(line.substr(0, 6));
		}
		// Finding End of Header Info
		else if (found_END != string::npos) {
			break;
		}
	}

	// Create GPS Navigation data holder
	map<int, vector<Rinex2Nav::DataGPS>> mapGPS;

	// Reading Navigation Data Body
	while (!infile.eof()) {
		line.clear();
		// Temporarily store line from input file
		getline(infile, line, '\n'); nlines++;
		if (line.find_first_not_of(' ') == std::string::npos) { continue; }
		// Adjust line spaces before adding to block
		if (nlines != 1) {
			line = line.substr(3, line.length());
		}
		block.push_back(line);
		// New block of navigation message
		if (nlines == 8) {
			// Now we must process the block of lines
			Rinex2Nav::DataGPS GPS = epochNavOrganizer(block);
			block.clear(); nlines = 0;
			// Add organized data to data holder
			// Save to Map: if PRN exists in map, then add NavInfo to vector of structs
			// Else add new PRN as key and GPS data structure as Value
			if (mapGPS.find(GPS.PRN) == mapGPS.end()) {
				// not found, therefore insert PRN and corresponding value
				vector<DataGPS> mapNavVector; mapNavVector.push_back(GPS);
				mapGPS.insert(pair<int, vector<DataGPS>>(GPS.PRN, mapNavVector));
			}
			else {
				// found, therefore add to existing PRN
				mapGPS[GPS.PRN].push_back(GPS);
			}
		}
	}
	// Update the attribute of Navigation Object
	_navDataGPS = mapGPS;
}
