/*
* FileIO.cpp
* A class for handling file input and output 
*  Created on: Jun 11, 2018
*      Author: Aaron Boda
*/

#include "pch.h"
#include "FileIO.h"
using namespace std;

// File Opener --> Initiates File pointer safely
void FileIO::fileSafeIn(string filename, ifstream& fin) {
	// Use input file stream, detect file
	ifstream inputfile(filename);
	// Check if file can be opened
	if (!inputfile.is_open())
	{
		perror("Error while opening file");
	}
	// Check is file can be read
	else if (inputfile.bad())
	{
		perror("Error while reading file");
	}
	else
	{
		// If no errors, we safely open the file...
		fin.open(filename);
	}
}

// A function to generate a LOG File for errors in Observation File
void FileIO::logger(string output_filename, string input_filename, ofstream& fout) {
	// Creating NEW LOG file
	fout.open(output_filename); fout.close();
	fout.open(output_filename, ios::app);
	// Preparing a format for LOG file
	fout << "-----------------------------------------------------------------------------------------\n";
	fout << "-----------------------------------------------------------------------------------------\n";
	fout << "THIS IS A LOG FILE FOR THE RINEX FILE READER.\n";
	fout << "THIS FILE CONTAINS RECORDS OF ANY UNEXPECTED BREAKS OR MISSING INFORMATION IN DATA FILES.\n";
	fout << "-----------------------------------------------------------------------------------------\n";
	fout << "-----------------------------------------------------------------------------------------\n";
	fout << "OBSERVATION FILE NAME: " << input_filename << "\n" << "\n";
	fout << std::left
		<< std::setw(30) << "EPOCH INFORMATION"
		<< std::setw(20) << "PRN"
		<< std::setw(20) << "MISSING OBSERVATION" << "\n";
	fout << "-----------------------------------------------------------------------------------------\n";
}

// A function to initialize the updated output datafile
void FileIO::fileSafeOut(string output_filename, ofstream& fout) {
	// Creating NEW LOG file
	fout.open(output_filename); fout.close();
	fout.open(output_filename, ios::app);
}

// Check Rinex File Version
void FileIO::checkRinexVersionType(int &rinex_version, int &rinex_type, std::ifstream &fin)
{
	const std::string sTokenVER = "RINEX VERSION / TYPE";
	std::string line; int nLines = 0;
	while (!fin.eof()) {
		nLines++;
		line.clear();
		getline(fin, line, '\n');
		size_t found_VER = line.find(sTokenVER);
		// RINEX Version
		if (found_VER != std::string::npos) {
			std::istringstream iss(line);
			std::vector<std::string> words{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };
			// Rinex Version
			if ((std::stod(words[0])) >= 3) {
				rinex_version = 3;
			}
			else if ((std::stod(words[0])) >= 2) {
				rinex_version = 2;
			}
			else {
				rinex_version = NULL;
			}
			// Rinex Type
			std::string type = words[3].substr(0, 1);
			if (type == "G") {
				rinex_type = 0;
			}
			else if (type == "M") {
				rinex_type = 1;
			}
			else {
				rinex_type = NULL;
			}
			words.clear();
			break;
		}
		// Fail safe
		if (nLines > 1) {
			// Rinex version usually provided in first line of file
			cout << "RINEX VERSION TYPE NOT FOUND IN FIRST LINE OF FILE! \n";
			break;
		}
	}
}
