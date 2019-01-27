/*
* Rinex3Nav.h
* Read and organize Rinex v3 ephemeris file in epochwise manner
*  Created on: Jun 12, 2018
*  Updated on: January 26, 2019
*      Author: Aaron Boda
*/

#include "pch.h"
#include "Rinex3Nav.h"
using namespace std;

// CONSTRUCTOR AND DESTRUCTOR DEFINITIONS
Rinex3Nav::Rinex3Nav() {}
Rinex3Nav::~Rinex3Nav() {}

// A function to help with organizing GPS header
// Works for the alpha/beta ionospheric constants and time correction
std::vector<double> headerHelperGPS(string line) {
	line = line.substr(5, 55);
	string word;
	vector<double> data;
	for (unsigned i = 0; i < line.length(); i += 12) {
		word = line.substr(i, 12);
		if (word.find_first_not_of(' ') == std::string::npos) { continue; }
		data.push_back(stod(replaceChars(word, 'D', 'e')));
		word.clear();
	}
	return data;
}

// Organizes Epoch Time Information into Vector
vector<double> rinex3EpochTimeOrganizer(string line) {
	vector<double> epochRecord;
	line = line.substr(3, 20);
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
vector<double> rinex3NavDataSplitter(string line) {
	// Split line every 19 spaces as allocated for parameters
	vector<double> data;
	string word;
	for (unsigned i = 0; i < line.length(); i += 19) {
		word = line.substr(i, 19);
		if (word.find_first_not_of(' ') == std::string::npos) { continue; }
		if (word.find('D') != std::string::npos) { data.push_back(stod(replaceChars(word, 'D', 'e'))); }
		if (word.find('E') != std::string::npos) { data.push_back(stod(replaceChars(word, 'E', 'e'))); }
		if (word.find('e') != std::string::npos) { data.push_back(stod(word)); }
		else { continue; }
		word.clear();
	}
	return data;
}

// Epoch Time Matcher, returns index of most appropriate Navigation vector
int Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGPS> NAV) {
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

// Epoch Time Matcher, returns index of most appropriate Navigation vector
int Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGLO> NAV) {
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

// Epoch Time Matcher, returns index of most appropriate Navigation vector
int Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGAL> NAV) {
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
Rinex3Nav::DataGPS epochNavOrganizerGPS(vector<string> block) {
	string sys = block[0].substr(0, 1);
	int prn = stoi(block[0].substr(1, 2));
	vector<double> epochInfo = rinex3EpochTimeOrganizer(block[0]);
	string line = block[0].substr(23, block[0].length());
	for (unsigned int i = 1; i < block.size(); i++) {
		line = line + block[i]; 
	}
	vector<double> parameters = rinex3NavDataSplitter(line);
	// Storing Values into GPS Data Structure
	Rinex3Nav::DataGPS GPS;
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
	GPS.transmission_time = parameters.at(27);
	GPS.fit_interval = parameters.at(28);
	return GPS;
}

// Reader for GPS navigation file
void Rinex3Nav::readGPS(std::ifstream& infile) {
	// String tokens to look for
	const string sTokenIALPHA = "ION ALPHA";
	const string sTokenIBETA = "ION BETA";
	const string sTokenIONO = "IONOSPHERIC CORR";
	const string sTokenGPSA = "GPSA";
	const string sTokenGPSB = "GPSB";
	const string sTokenCORR = "TIME SYSTEM CORR";
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
		size_t found_IONO = line.find(sTokenIONO);
		size_t found_END = line.find(sTokenEND);
		size_t found_CORR = line.find(sTokenCORR);
		size_t found_COM = line.find(sTokenCOM);

		// Finding Comments, meaning skip!
		if (found_COM != string::npos) {
			continue;
		}
		// Finding Ionophseric Constants as per new format
		else if (found_IONO != string::npos) {
			size_t found_GPSA = line.find(sTokenGPSA);
			size_t found_GPSB = line.find(sTokenGPSB);
			if (found_GPSA != string::npos) {
				_headerGPS.ialpha = headerHelperGPS(line);
			}
			if (found_GPSB != string::npos) {
				_headerGPS.ibeta = headerHelperGPS(line);
			}
		}
		// Finding GPS to UTC Time Correction
		else if (found_CORR != string::npos) {
			size_t found_GPUT = line.find("GPUT");
			_headerGPS.GPUT = headerHelperGPS(line);
		}
		// Finding End of Header Info
		else if (found_END != string::npos) {
			break;
		}
	}

	// Create GPS Navigation data holder
	map<int, vector<Rinex3Nav::DataGPS>> mapGPS;

	// Reading Navigation Data Body
	while (!infile.eof()) {
		line.clear();
		// Temporarily store line from input file
		getline(infile, line, '\n'); nlines++;
		if (line.find_first_not_of(' ') == std::string::npos) { continue; }
		// Adjust line spaces before adding to block
		if (nlines != 1) {
			line = line.substr(4, line.length());
		}
		block.push_back(line);
		// New block of navigation message
		if (nlines == 8) {
			// Now we must process the block of lines
			Rinex3Nav::DataGPS GPS = epochNavOrganizerGPS(block);
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
	_navGPS = mapGPS;
}

// Navigation Body Organizer for GLONASS Navigation File
Rinex3Nav::DataGLO epochNavOrganizerGLO(vector<string> block) {
	string sys = block[0].substr(0, 1);
	int prn = stoi(block[0].substr(1, 2));
	vector<double> epochInfo = rinex3EpochTimeOrganizer(block[0]);
	string line = block[0].substr(23, block[0].length());
	for (unsigned int i = 1; i < block.size(); i++) {
		line = line + block[i];
	}
	vector<double> parameters = rinex3NavDataSplitter(line);
	// Storing Values into GPS Data Structure
	Rinex3Nav::DataGLO GLO;
	GLO.PRN = prn;
	GLO.epochInfo = epochInfo;
	GLO.gpsTime = gpsTime(epochInfo);
	GLO.clockBias = parameters.at(0);
	GLO.relFreqBias = parameters.at(1);
	GLO.messageFrameTime = parameters.at(2);
	GLO.satPosX = parameters.at(3);
	GLO.satVelX = parameters.at(4);
	GLO.satAccX = parameters.at(5);
	GLO.satHealth = parameters.at(6);
	GLO.satPosY = parameters.at(7);
	GLO.satVelY = parameters.at(8);
	GLO.satAccY = parameters.at(9);
	GLO.freqNum = parameters.at(10);
	GLO.satPosZ = parameters.at(11);
	GLO.satVelZ = parameters.at(12);
	GLO.satAccZ = parameters.at(13);
	GLO.infoAge = parameters.at(14);
	return GLO;
}

// Reader for Glonass navigation file
// Parameters are in ECEF Greenwich coordinate system PZ - 90
void Rinex3Nav::readGLO(std::ifstream& infile) {
	// String tokens to look 
	const string sTokenCORR = "CORR TO SYSTEM TIME";
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
		size_t found_LEAP = line.find(sTokenLEAP);
		size_t found_END = line.find(sTokenEND);
		size_t found_CORR = line.find(sTokenCORR);
		size_t found_COM = line.find(sTokenCOM);

		// Finding Comments, meaning skip!
		if (found_COM != string::npos) {
			continue;
		}
		// Finding Time Correction
		else if (found_CORR != string::npos) {
			line = line.substr(0, 60);
			// Splitting words in the line
			istringstream iss(line);
			vector<string> dataS{ istream_iterator<string>{iss}, istream_iterator<string>{} };
			for (string s : dataS) { _headerGLO.TimeCorr.push_back(stod(s)); }
			dataS.clear();
		}
		// Finding Leap Second
		else if (found_LEAP != string::npos) {
			line = line.substr(0, 7);
			_headerGLO.leapSec = stod(line);
		}
		// Finding End of Header Info
		else if (found_END != string::npos) {
			break;
		}
	}

	// Create GLO Navigation data holder
	map<int, vector<Rinex3Nav::DataGLO>> mapGLO;

	// Reading Navigation Data Body
	while (!infile.eof()) {
		line.clear();
		// Temporarily store line from input file
		getline(infile, line, '\n'); nlines++;
		if (line.find_first_not_of(' ') == std::string::npos) { continue; }
		// Adjust line spaces before adding to block
		if (nlines != 1) {
			line = line.substr(4, line.length());
		}
		block.push_back(line);
		// New block of navigation message
		if (nlines == 4) {
			// Now we must process the block of lines
			Rinex3Nav::DataGLO GLO = epochNavOrganizerGLO(block);
			block.clear(); nlines = 0;
			// Add organized data to data holder
			// Save to Map: if PRN exists in map, then add NavInfo to vector of structs
			// Else add new PRN as key and GLO data structure as Value
			if (mapGLO.find(GLO.PRN) == mapGLO.end()) {
				// not found, therefore insert PRN and corresponding value
				vector<DataGLO> mapNavVector; mapNavVector.push_back(GLO);
				mapGLO.insert(pair<int, vector<DataGLO>>(GLO.PRN, mapNavVector));
			}
			else {
				// found, therefore add to existing PRN
				mapGLO[GLO.PRN].push_back(GLO);
			}
		}
	}
	// Update the attribute of Navigation Object
	_navGLO = mapGLO;
}

// Navigation Body Organizer for GAL Navigation File
Rinex3Nav::DataGAL epochNavOrganizerGAL(vector<string> block) {
	string sys = block[0].substr(0, 1);
	int prn = stoi(block[0].substr(1, 2));
	vector<double> epochInfo = rinex3EpochTimeOrganizer(block[0]);
	string line = block[0].substr(23, block[0].length());
	for (unsigned int i = 1; i < block.size(); i++) {
		line = line + block[i];
	}
	vector<double> parameters = rinex3NavDataSplitter(line);
	// Storing Values into GAL Data Structure
	Rinex3Nav::DataGAL GAL;
	GAL.PRN = prn;
	GAL.epochInfo = epochInfo;
	GAL.gpsTime = gpsTime(epochInfo);
	GAL.clockBias = parameters.at(0);
	GAL.clockDrift = parameters.at(1);
	GAL.clockDriftRate = parameters.at(2);
	GAL.IOD = parameters.at(3);
	GAL.Crs = parameters.at(4);
	GAL.Delta_n = parameters.at(5);
	GAL.Mo = parameters.at(6);
	GAL.Cuc = parameters.at(7);
	GAL.Eccentricity = parameters.at(8);
	GAL.Cus = parameters.at(9);
	GAL.Sqrt_a = parameters.at(10);
	GAL.TOE = parameters.at(11);
	GAL.Cic = parameters.at(12);
	GAL.OMEGA = parameters.at(13);
	GAL.CIS = parameters.at(14);
	GAL.Io = parameters.at(15);
	GAL.Crc = parameters.at(16);
	GAL.Omega = parameters.at(17);
	GAL.Omega_dot = parameters.at(18);
	GAL.IDOT = parameters.at(19);
	GAL.GAL_week = parameters.at(21);
	GAL.SISA = parameters.at(22);
	GAL.svHealth = parameters.at(23);
	GAL.BGD_E5a = parameters.at(24);
	GAL.BGD_E5b = parameters.at(25);
	GAL.transmission_time = parameters.at(26);
	return GAL;
}

// Reader for Galileo navigation file
void Rinex3Nav::readGAL(std::ifstream& infile) {
	// String tokens to look 
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
		size_t found_LEAP = line.find(sTokenLEAP);
		size_t found_END = line.find(sTokenEND);
		size_t found_COM = line.find(sTokenCOM);

		// Finding Comments, meaning skip!
		if (found_COM != string::npos) {
			continue;
		}
		// Finding Leap Second
		else if (found_LEAP != string::npos) {
			line = line.substr(0, 7);
			_headerGLO.leapSec = stod(line);
		}
		// Finding End of Header Info
		else if (found_END != string::npos) {
			break;
		}
	}

	// Create GAL Navigation data holder
	map<int, vector<Rinex3Nav::DataGAL>> mapGAL;

	// Reading Navigation Data Body
	while (!infile.eof()) {
		line.clear();
		// Temporarily store line from input file
		getline(infile, line, '\n'); nlines++;
		if (line.find_first_not_of(' ') == std::string::npos) { continue; }
		// Adjust line spaces before adding to block
		if (nlines != 1) {
			line = line.substr(4, line.length());
		}
		block.push_back(line);
		// New block of navigation message
		if (nlines == 8) {
			// Now we must process the block of lines
			Rinex3Nav::DataGAL GAL = epochNavOrganizerGAL(block);
			block.clear(); nlines = 0;
			// Add organized data to data holder
			// Save to Map: if PRN exists in map, then add NavInfo to vector of structs
			// Else add new PRN as key and GAL data structure as Value
			if (mapGAL.find(GAL.PRN) == mapGAL.end()) {
				// not found, therefore insert PRN and corresponding value
				vector<DataGAL> mapNavVector; mapNavVector.push_back(GAL);
				mapGAL.insert(pair<int, vector<DataGAL>>(GAL.PRN, mapNavVector));
			}
			else {
				// found, therefore add to existing PRN
				mapGAL[GAL.PRN].push_back(GAL);
			}
		}
	}
	// Update the attribute of Navigation Object
	_navGAL = mapGAL;
}

// Reader for GPS navigation file
void Rinex3Nav::readMixed(std::ifstream& infile) {
	// String tokens to look for
	const string sTokenIALPHA = "ION ALPHA";
	const string sTokenIBETA = "ION BETA";
	const string sTokenIONO = "IONOSPHERIC CORR";
	const string sTokenGPSA = "GPSA";
	const string sTokenGPSB = "GPSB";
	const string sTokenCORR = "TIME SYSTEM CORR";
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
		size_t found_IONO = line.find(sTokenIONO);
		size_t found_END = line.find(sTokenEND);
		size_t found_CORR = line.find(sTokenCORR);
		size_t found_COM = line.find(sTokenCOM);

		// Finding Comments, meaning skip!
		if (found_COM != string::npos) {
			continue;
		}
		// Finding Ionophseric Constants as per new format
		else if (found_IONO != string::npos) {
			size_t found_GPSA = line.find(sTokenGPSA);
			size_t found_GPSB = line.find(sTokenGPSB);
			if (found_GPSA != string::npos) {
				_headerGPS.ialpha = headerHelperGPS(line);
			}
			if (found_GPSB != string::npos) {
				_headerGPS.ibeta = headerHelperGPS(line);
			}
		}
		// Finding GPS to UTC Time Correction
		else if (found_CORR != string::npos) {
			size_t found_GPUT = line.find("GPUT");
			_headerGPS.GPUT = headerHelperGPS(line);
		}
		// Finding End of Header Info
		else if (found_END != string::npos) {
			break;
		}
	}

	// Create GPS Navigation data holder
	map<int, vector<Rinex3Nav::DataGPS>> mapGPS;
	map<int, vector<Rinex3Nav::DataGLO>> mapGLO;
	map<int, vector<Rinex3Nav::DataGAL>> mapGAL;

	// Reading Navigation Data Body
	while (!(infile >> std::ws).eof()) {
		// *** Deal with end of file error
		if (infile.fail()) { break; }
		// ***

		// Temporarily store line from input file
		line.clear();
		getline(infile, line, '\n');

		// Constellation identifier
		string ID = line.substr(0, 1);

		// GPS
		if(ID.find('G') != std::string::npos) {
			nlines = 0;
			while ((!(infile >> std::ws).eof()) || (nlines <= 8)) {
				// *** Deal with end of file error
				if (infile.fail()) { break; }
				// ***

				if (line.find_first_not_of(' ') == string::npos) { continue; }
				block.push_back(line); nlines++;

				// New block of navigation message
				if (nlines == 8) {
					// Now we must process the block of lines
					Rinex3Nav::DataGPS GPS = epochNavOrganizerGPS(block);
					block.clear(); line.clear();
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
					break;
				}
				if (nlines < 8) {
					line.clear();
					getline(infile, line, '\n');
				}
			}
		}

		// GALILEO
		if (ID.find('E') != std::string::npos) {
			nlines = 0;
			while ((!(infile >> std::ws).eof()) || (nlines <= 8)) {
				// *** Deal with end of file error
				if (infile.fail()) { break; }
				// ***

				if (line.find_first_not_of(' ') == string::npos) { continue; }
				block.push_back(line); nlines++;

				// New block of navigation message
				if (nlines == 8) {
					// Now we must process the block of lines
					Rinex3Nav::DataGAL GAL = epochNavOrganizerGAL(block);
					block.clear(); line.clear(); 
					// Add organized data to data holder
					// Save to Map: if PRN exists in map, then add NavInfo to vector of structs
					// Else add new PRN as key and GAL data structure as Value
					if (mapGAL.find(GAL.PRN) == mapGAL.end()) {
						// not found, therefore insert PRN and corresponding value
						vector<DataGAL> mapNavVector; mapNavVector.push_back(GAL);
						mapGAL.insert(pair<int, vector<DataGAL>>(GAL.PRN, mapNavVector));
					}
					else {
						// found, therefore add to existing PRN
						mapGAL[GAL.PRN].push_back(GAL);
					}
					break;
				}
				if (nlines < 8) {
					line.clear();
					getline(infile, line, '\n');
				}
			}
		}

		// GLONASS
		if (ID.find('R') != std::string::npos) {
			nlines = 0;
			while ((!(infile >> std::ws).eof()) || (nlines <= 4)) {
				// *** Deal with end of file error
				if (infile.fail()) { break; }
				// ***

				if (line.find_first_not_of(' ') == string::npos) { continue; }
				block.push_back(line); nlines++;

				// New block of navigation message
				if (nlines == 4) {
					// Now we must process the block of lines
					Rinex3Nav::DataGLO GLO = epochNavOrganizerGLO(block);
					block.clear(); line.clear();
					// Add organized data to data holder
					// Save to Map: if PRN exists in map, then add NavInfo to vector of structs
					// Else add new PRN as key and GLO data structure as Value
					if (mapGLO.find(GLO.PRN) == mapGLO.end()) {
						// not found, therefore insert PRN and corresponding value
						vector<DataGLO> mapNavVector; mapNavVector.push_back(GLO);
						mapGLO.insert(pair<int, vector<DataGLO>>(GLO.PRN, mapNavVector));
					}
					else {
						// found, therefore add to existing PRN
						mapGLO[GLO.PRN].push_back(GLO);
					}
					break;
				}
				if (nlines < 4) {
					line.clear();
					getline(infile, line, '\n');
				}
			}
		}
	}

	// Update the attribute of Navigation Objects
	_navGPS = mapGPS;
	_navGLO = mapGLO;
	_navGAL = mapGAL;
}