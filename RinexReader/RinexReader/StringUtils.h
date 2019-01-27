#pragma once
/*
* StringUtils.h
* Custom functions used for string manipulation
*  Created on: Jun 10, 2018
*      Author: Aaron Boda
*/

#include "pch.h"

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

// Functions
std::string replaceChars(std::string str, char ch1, char ch2);
void eraseSubStr(std::string & mainStr, const std::string & toErase);
std::string HHMMSS(double hours, double mins, double secs);

#endif /* STRINGUTILS_H_ */
