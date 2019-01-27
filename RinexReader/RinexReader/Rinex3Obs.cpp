/*
* Rinex3Obs.cpp
* Read and organize Rinex v3 file in epochwise manner
*  Created on: Jun 10, 2018
*      Author: Aaron Boda
*/ 

#include "pch.h"
#include "Rinex3Obs.h"
using namespace std;

// CONSTRUCTOR AND DESTRUCTOR DEFINITIONS
Rinex3Obs::Rinex3Obs() {}
Rinex3Obs::~Rinex3Obs() {}

// A function to organize specific observation types (for eg: prn - > pseudorange map) 
// PRN - > Pseudorange Map is used for satellite position
map<int, double> Rinex3Obs::specificObsMapper(map<int, vector<double>> obsSAT, vector<string> obsTypes, string specificObs) {
	map<int, double> rangeMap;
	map<int, vector<double>>::iterator it = obsSAT.begin();
	size_t ind = std::find(obsTypes.begin(), obsTypes.end(), specificObs) - obsTypes.begin();
	if (ind >= obsTypes.size()) {
		cout << "Requested C1 Observation is Unavailable" << endl;
	}
	else if (ind < obsTypes.size()) {
		for (it = obsSAT.begin(); it != obsSAT.end(); ++it) {
			rangeMap.insert(std::pair<int, double>(it->first, it->second.at(ind)));
		}
	}
	return rangeMap;
}

// A function to organize observation types as stated in header of rinex observation file 
map<string, vector<string>> obsTypesHeader(vector<string> block) {
	// Initializing variables to hold information
	map<string, vector<string>> types;
	// Satellite system identifier
	string sys;
	// Number of observation types
	int nTypes;
	// Temporary vars
	string line; vector<string> temp;
    for (unsigned int i = 0; i < block.size(); i++) {
		line = block[i];
		// Splitting words in the line
		istringstream iss(line);
		vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
		// Challenge is to organize based on satellite system identifier
		// First letter of line gives the satellite system
		// If number of types > 13, types extended on next line
		size_t sLength = words[0].length();
		if (sLength == 1) {
			sys = words[0];
			nTypes = stoi(words[1]);
			if (nTypes > 13) {
				i++;
				line = line + block[i];
				words.clear();
				istringstream iss2(line);
				copy(istream_iterator<string>(iss2), istream_iterator<string>(), back_inserter(words));
			}
            for (unsigned int i = 2; i < words.size(); i++) {
				temp.push_back(words[i]);
			}
			types[sys] = temp;
			temp.clear();
		}
	}
	return types;
}

// Extracts and stores the header information from Rinex v3 File
void Rinex3Obs::obsHeader(ifstream& infile) {
	// String tokens to look for
	const string sTokenVER = "RINEX VERSION / TYPE";
	const string sTokenPOS = "APPROX POSITION XYZ";
	const string sTokenDEL = "ANTENNA: DELTA";
	const string sTokenOBS = "SYS / # / OBS TYPES";
	const string sTokenCOM = "COMMENT";
	const string sTokenFIR = "TIME OF FIRST OBS";
	const string sTokenLAS = "TIME OF LAST OBS";
	const string sTokenEND = "END OF HEADER";
	// A vector to hold block of sentences pertaining to observation types
	vector<string> types;
	// To hold contents of a line from input file
	string line;

	// Reading line by line...
	while (!infile.eof()) {
		line.clear();
		// Temporarily store line from input file
		getline(infile, line, '\n');
		// Looking for keywords in Header Part...
		size_t found_VER = line.find(sTokenVER);
		size_t found_POS = line.find(sTokenPOS);
		size_t found_DEL = line.find(sTokenDEL);
		size_t found_OBS = line.find(sTokenOBS);
		size_t found_COM = line.find(sTokenCOM);
		size_t found_FIR = line.find(sTokenFIR);
		size_t found_LAS = line.find(sTokenLAS);
		size_t found_END = line.find(sTokenEND);

		// Finding Comments, meaning skip!
		if (found_COM != string::npos) {
			continue;
		}
		// RINEX Version
		else if (found_VER != string::npos) {
			istringstream iss(line);
			// Rinex type should be stored in 4th word of line
			vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
			_Header.rinexType = words[3];
			words.clear();
		}
		// Approximate Position
		else if (found_POS != string::npos) {
			istringstream iss(line);
			copy(istream_iterator<double>(iss),
				istream_iterator<double>(),
				back_inserter(_Header.approxPosXYZ));
			// Adding a term for Clock Offset
			_Header.approxPosXYZ.push_back(0);
		}
		// Antenna Delta
		else if (found_DEL != string::npos) {
			istringstream iss(line);
			copy(istream_iterator<double>(iss),
				istream_iterator<double>(),
				back_inserter(_Header.antDeltaHEN));
		}
		// Types of Observations
		else if (found_OBS != string::npos) {
			// Removing identifier from block of lines
			eraseSubStr(line, sTokenOBS);
			// Storing in vector for processing later
			types.push_back(line);
		}
		// Time of First Obs
		else if (found_FIR != string::npos) {
			istringstream iss(line);
			copy(istream_iterator<double>(iss),
				istream_iterator<double>(),
				back_inserter(_Header.firstObsTime));
		}
		// Time of Last Obs
		else if (found_LAS != string::npos) {
			istringstream iss(line);
			copy(istream_iterator<double>(iss),
				istream_iterator<double>(),
				back_inserter(_Header.lastObsTime));
		}
		// Finding End of Header Info
		else if (found_END != string::npos) {
			break;
		}
	}
	// Organizing the observation types
	_Header.obsTypes = obsTypesHeader(types);
	if (_Header.obsTypes.count("G") > 0) { _obsTypesGPS = _Header.obsTypes["G"]; }
	if (_Header.obsTypes.count("R") > 0) { _obsTypesGLO = _Header.obsTypes["R"]; }
	if (_Header.obsTypes.count("E") > 0) { _obsTypesGAL = _Header.obsTypes["E"]; }
}

// Splits Epoch Information
vector<double> rinex3EpochRecordOrganizer(string line) {
	vector<double> epochRecord;
	// Splitting words in the line
	istringstream iss(line);
	vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
	for (string s : words) {
		epochRecord.push_back(stod(s));
	}
	// Return: Vector containing the Epoch Info
	return epochRecord;
}

// Epoch Satellite Observation Data Organizer
void rinex3SatObsOrganizer(string line, map<string, map<int, vector<double>>>& data) {
	// First word contains satellite system and number
	string sys = line.substr(0, 1);
	int prn = stoi(line.substr(1, 2));
	// Rest of the words should contain observations
	// Obs format is 14.3, 14 for Obs and 3 for S/N
	line = line.substr(3, line.length());
	string word;
	vector<double> obs;
	for (unsigned i = 0; i < line.length(); i += 16) {
		word = line.substr(i, 14);
		if (word.find_first_not_of(' ') == string::npos) {
			word = "0";
		}
		obs.push_back(stod(word));
		word.clear();
	}
	// Update map data
	data[sys].insert(make_pair(prn, obs));
}

// This function is used to organize the string block of epoch info into data structure
void obsOrganizer(vector<string> block, Rinex3Obs::ObsEpochInfo& obs) {
	// First line contains epoch time information and receiver clock offset
	obs.epochRecord = rinex3EpochRecordOrganizer(block[0]);
	obs.recClockOffset = obs.epochRecord.back();
	// Organize satellite observations in data structure
	map<string, map<int, vector<double>>> data;
	string line;
    for (unsigned int i = 1; i < block.size(); i++) {
		line.clear(); line = block[i];
		rinex3SatObsOrganizer(line, data);
	}
	obs.observations = data;
	obs.numSatsGAL = 0;
	obs.numSatsGLO = 0;
	obs.numSatsGPS = 0;
	map<string, map<int, vector<double>>>::iterator it;
	it = data.find("E");
	if (it != data.end()) { obs.numSatsGAL = static_cast<int>(it->second.size()); }
	it = data.find("R");
	if (it != data.end()) { obs.numSatsGLO = static_cast<int>(it->second.size()); }
	it = data.find("G");
	if (it != data.end()) { obs.numSatsGPS = static_cast<int>(it->second.size()); }
}

// Setting observation attributes for each satellite constellations
void Rinex3Obs::setObservations(map<string, map<int, vector<double>>> observations) {
	if (observations.count("G") > 0) { _obsGPS.clear(); _obsGPS = observations["G"]; }
	if (observations.count("R") > 0) { _obsGLO.clear(); _obsGLO = observations["R"]; }
	if (observations.count("E") > 0) { _obsGAL.clear(); _obsGAL = observations["E"]; }
}

// This function extracts and stores epochwise observations from file
void Rinex3Obs::obsEpoch(ifstream& infile) {
	// Rinex v3 special identifier for new epoch of observations
	const string sTokenEpoch = ">";
	// Collect the block of observation lines into a vector
	int nSatsEpoch = 0; int nLinesEpoch = 0; vector<int> blockFirstLine;
	streampos pos;
	string line;
	vector<string> block;
	// Reading line by line...
	while (!(infile >> std::ws).eof()) {
		// *** Deal with end of file error
		if (infile.fail()) { break; }
		// ***
		line.clear();
		pos = infile.tellg();
		// Temporarily store line from input file
		getline(infile, line); nLinesEpoch++;

		if (line.find_first_not_of(' ') == string::npos) { continue; }
		// Look for special identifier in line
		size_t found_ID = line.find(sTokenEpoch);
		if ((found_ID != string::npos)) {
			if (block.size() == 0) {
				eraseSubStr(line, sTokenEpoch);
				// Find number of sats in epoch
				istringstream iss(line);
				vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
				for (string s : words) {
					blockFirstLine.push_back(stoi(s));
				}
				nSatsEpoch = blockFirstLine[7];
			}
			else {
				infile.seekg(pos);
				break;
			}
		}
		block.push_back(line);
		// Fail-safe epoch quitter
		if (nLinesEpoch == nSatsEpoch+1) { break; }
	}
	// Now we must process the block of lines
	_EpochObs.clear();
	obsOrganizer(block, _EpochObs);
	_EpochObs.gpsTime = gpsTime(_EpochObs.epochRecord);
	// Update observation attributes
	setObservations(_EpochObs.observations);
}

// To clear contents in observation data structure
void Rinex3Obs::clear(Rinex3Obs::ObsEpochInfo& obs) {
	obs.epochRecord.clear();
	obs.numSatsGAL = NULL;
	obs.numSatsGLO = NULL;
	obs.numSatsGPS = NULL;
	obs.observations.clear();
	obs.recClockOffset = NULL;
}

// To clear contents in observation data structure
void Rinex3Obs::clear(Rinex3Obs::ObsHeaderInfo& header) {
	header.antDeltaHEN.clear();
	header.approxPosXYZ.clear();
	header.firstObsTime.clear();
	header.lastObsTime.clear();
	header.obsTypes.clear();
	header.rinexType.clear();
}
