/*
* StringUtils.cpp
* Custom functions used for string manipulation
*  Created on: Jun 10, 2018
*      Author: Aaron Boda
*/

#include "pch.h"
#include "StringUtils.h"

using namespace std;

// A function used to replace a character ch1 in a string with another character ch2
string replaceChars(string str, char ch1, char ch2) {
	for (unsigned i = 0; i < str.length(); ++i) {
		if (str[i] == ch1)
			str[i] = ch2;
	}
	return str;
}

// A function to erase substring (first occurence of) from main string
void eraseSubStr(string & mainStr, const string & toErase) {
	// Search for the substring in string
	size_t pos = mainStr.find(toErase);
	if (pos != string::npos){
		// If found then erase it from string
		mainStr.erase(pos, toErase.length());
	}
}

// A function to define time in HH:MM:SS format
string HHMMSS(double hours, double mins, double secs) {
	stringstream ss;
	ss << std::setw(2) << std::setfill('0') << (int)hours << ":";
	ss << std::setw(2) << std::setfill('0') << (int)mins << ":";
	ss << std::setw(2) << std::setfill('0') << (int)secs;
	string hms = ss.str();
	return hms;
}