/*
* Rinex2Obs.cpp
* Read and organize Rinex v2 observation file in epochwise manner
*  Created on: Jun 16, 2018
*      Author: Aaron Boda
*/

#include "pch.h"
#include "Rinex2Obs.h"

using namespace std;

// CONSTRUCTOR AND DESTRUCTOR DEFINITIONS
Rinex2Obs::Rinex2Obs() {}
Rinex2Obs::~Rinex2Obs() {}

// A function to organize specific observation type (for ex: prn - > pseudorange map) 
// PRN - > Pseudorange Map is used for satellite position
map<int, double> Rinex2Obs::specificObsMapper(map<int, vector<double>> _obsGPS, vector<string> obsTypes, string specificObs) {
	map<int, double> rangeMap;
	map<int, vector<double>>::iterator it = _obsGPS.begin();
	size_t ind = std::find(obsTypes.begin(), obsTypes.end(), specificObs) - obsTypes.begin();
	if (ind >= obsTypes.size()) {
		cout << "Requested C1 Observation is Unavailable" << endl;
	}
	else if (ind < obsTypes.size()) {
		for (it = _obsGPS.begin(); it != _obsGPS.end(); ++it) {
			rangeMap.insert(std::pair<int, double>(it->first, it->second.at(ind)));
		}
	}
	return rangeMap;
}

// This function extracts and stores the header information from Rinex v3 File
void Rinex2Obs::obsHeader(ifstream& infile) {
	// String tokens to look for
	const string sTokenVER = "RINEX VERSION / TYPE";
	const string sTokenPOS = "APPROX POSITION XYZ";
	const string sTokenDEL = "ANTENNA: DELTA";
	const string sTokenOBS = "# / TYPES OF OBSERV";
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
			_header.rinexType = words[3];
			words.clear();
		}
		// Approximate Position
		else if (found_POS != string::npos) {
			istringstream iss(line);
			copy(istream_iterator<double>(iss),
				istream_iterator<double>(),
				back_inserter(_header.approxPosXYZ));
			// Adding a term for Clock Offset
			_header.approxPosXYZ.push_back(0);
		}
		// Antenna Delta
		else if (found_DEL != string::npos) {
			istringstream iss(line);
			copy(istream_iterator<double>(iss),
				istream_iterator<double>(),
				back_inserter(_header.antDeltaHEN));
		}
		// Types of Observations
		else if (found_OBS != string::npos) {
			// Removing identifier from block of lines
			eraseSubStr(line, sTokenOBS);
			// Sepearate words from line and add to vector
			istringstream iss(line);
			vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
			_header.nObsTypes = stoi(words[0]);
			for (int i = 1; i < (int)words.size(); i++) {
				_header.obsTypes.push_back(words[i]);
			}
			_obsTypesGPS = _header.obsTypes;
		}
		// Time of First Obs
		else if (found_FIR != string::npos) {
			istringstream iss(line);
			copy(istream_iterator<double>(iss),
				istream_iterator<double>(),
				back_inserter(_header.firstObsTime));
		}
		// Time of Last Obs
		else if (found_LAS != string::npos) {
			istringstream iss(line);
			copy(istream_iterator<double>(iss),
				istream_iterator<double>(),
				back_inserter(_header.lastObsTime));
		}
		// Finding End of Header Info
		else if (found_END != string::npos) {
			break;
		}
	}
}

// Splits Epoch Information
vector<double> rinex2EpochRecordOrganizer(string line) {
	line = line.substr(0, 26);
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

// Splits PRN Information
vector<int> rinex2SatOrganizer(string line) {
	vector<int> sats;
	string word;
	for (unsigned i = 0; i < line.length(); i += 3) {
		word = line.substr(i, 3);
		if (word.find_first_not_of(' ') == string::npos) {
			word = "0"; continue;
		}
		word = replaceChars(word, 'G', ' ');
		sats.push_back(stoi(word));
		word.clear();

	}
	return sats;
}

// Epoch Satellite Observation Data Organizer
void rinex2ObsOrganizer(vector<string> block, vector<int> satellites, int nObsTypes, map<int, vector<double>>& mapSatObs, map<int, vector<int>>& mapObsLLI, map<int, vector<int>>& mapObsSS) {
	// If nObsTypes is more than 5, observations take up two lines per satellite
	vector<string> nBlock;
	if (nObsTypes > 5) {
		for (int i = 0; i < (int)block.size(); i++) {
			string line = block[i]; i++;
			line = line + block[i];
			nBlock.push_back(line);
		}
	}
	else {
		nBlock = block;
	}
	// Create Observation Data Holder
	// prn -> vector of observations
	mapSatObs.clear(); mapObsLLI.clear(); mapObsSS.clear();
	for (int j = 0; j < (int)nBlock.size(); j++) {
		vector<double> OBS;
		vector<int> LLI;
		vector<int> SS;
		string obsTok; string lliTok; string ssTok;
		string line = nBlock[j];
		for (unsigned i = 0; i < line.length(); i += 16) {
			if (line.length() <= (i + 14)) {
				obsTok = line.substr(i, line.length() - i);
				lliTok = "0";
				ssTok = "0";
			}
			else {
				obsTok = line.substr(i, 14);
				lliTok = line.substr(i + 14, 1);
				ssTok = line.substr(i + 15, 1);
			}
			if (obsTok.find_first_not_of(' ') == string::npos) { obsTok = "0"; }
			OBS.push_back(stod(obsTok));
			obsTok.clear();
			if (lliTok.find_first_not_of(' ') == string::npos) { lliTok = "0"; }
			LLI.push_back(stoi(lliTok));
			lliTok.clear();
			if (ssTok.find_first_not_of(' ') == string::npos) { ssTok = "0"; }
			SS.push_back(stoi(ssTok));
			ssTok.clear();
		}
		mapSatObs.insert(make_pair(satellites[j], OBS));
		mapObsLLI.insert(make_pair(satellites[j], LLI));
		mapObsSS.insert(make_pair(satellites[j], SS));
	}
}

// This function extracts and stores epochwise observations from file
void Rinex2Obs::obsEpoch(ifstream& infile, ofstream& logfile, int nObsTypes) {
	// Rinex v2 special identifier for new epoch of observations
	const string sTokenEpoch = "G";
	const string sTokenCOM = "COMMENT";
	// Collect the block of observation lines into a vector
	streampos pos;
	string line;
	vector<string> block;
	int nLines = 0, bLines = 0;
	// Reading line by line...

	while (!(infile >> std::ws).eof()) {
		// *** Deal with end of file error
		if (infile.fail()) { break; }
		// *** 
		line.clear();
		pos = infile.tellg();
		// Temporarily store line from input file
		getline(infile, line);

		// Taking care of line length
		size_t lsize = line.size();
		size_t bsize = 0;
		if (lsize < 80) {
			bsize = 80 - lsize;
			string blanks(bsize, ' ');
			line = line + blanks;
		}

		// Taking care of empty lines within obs epoch
		if (line.find_first_not_of(' ') == string::npos) {
			string blanks(80, ' ');
			block.push_back(blanks);
			nLines++;
			if (nLines == bLines) { break; }
			continue;
		}

		// Look for special identifier in line
		size_t found_ID = line.find(sTokenEpoch);
		size_t found_COM = line.find(sTokenCOM);
		if ((found_COM != string::npos)) { continue; }
		if ((found_ID != string::npos)) {
			if (block.size() == 0) {
				// Extract Epoch Information 
				_obsDataGPS.epochRecord = rinex2EpochRecordOrganizer(line);
				// Determine if PRN info carries to next line based on nObsTypes
				string tokSats = line.substr(29, 3);
				_obsDataGPS.nSats = stoi(tokSats);
				// Store Receiver Clock Offset if available
				string rco = "0";
				if (line.length() == 80) {
					rco = line.substr(68, 12);
					if (rco.find_first_not_of(' ') == string::npos) {
						rco = "0";
					}
				}
				_obsDataGPS.recClockOffset = stod(rco);
				// Read next line if necessary (PRN info carries to next line)
				line = line.substr(32, 36);
				if (_obsDataGPS.nSats > 12) {
					string line2;
					getline(infile, line2, '\n');
					line2 = line2.substr(32, 36);
					line = line + line2;
				}
				// Create vector of PRN's in current epoch
				_obsDataGPS.sats = rinex2SatOrganizer(line);
				// Number of possible lines in epoch block
				bLines = _obsDataGPS.nSats;
				if (nObsTypes > 5) { bLines = bLines * 2; }
			}
		}
		else {
			if (bLines == 0) { continue; }
			block.push_back(line);
			nLines++;
			if (nLines == bLines) { break; }
		}
	}
	// Now we must process the block of lines
	map<int, vector<double>> mapSatObs;
	map<int, vector<int>> mapObsLLI;
	map<int, vector<int>> mapObsSS;
	rinex2ObsOrganizer(block, _obsDataGPS.sats, nObsTypes, mapSatObs, mapObsLLI, mapObsSS);
	_obsDataGPS.observations = mapSatObs;
	_obsDataGPS.LLI = mapObsLLI;
	_obsDataGPS.SS = mapObsSS;
	_obsDataGPS.gpsTime = gpsTime(_obsDataGPS.epochRecord);
	_obsGPS = _obsDataGPS.observations;
}

// To clear contents in observation data structure
void Rinex2Obs::clearObs() {
	_obsDataGPS.epochRecord.clear();
	_obsDataGPS.gpsTime = NULL;
	_obsDataGPS.nSats = NULL;
	_obsDataGPS.observations.clear();
	_obsDataGPS.recClockOffset = NULL;
	_obsDataGPS.sats.clear();
}

// To clear contents in _header data structure
void Rinex2Obs::clearHeader() {
	_header.antDeltaHEN.clear();
	_header.approxPosXYZ.clear();
	_header.firstObsTime.clear();
	_header.lastObsTime.clear();
	_header.obsTypes.clear();
	_header.rinexType.clear();
}
