# RinexReader

## Synopsis

RINEX file format is used to store GNSS observations in a standardized format. 
You can find more info here: https://kb.igs.org/hc/en-us/articles/115003980188-RINEX-2-11

## Motivation

Making a RINEX Reader often becomes a redundant and time-consuming part for someone new starting out with GNSS processing. This utility will help one read the observation files to use for their GNSS solution processing. 

## Installation

If you have Visual Studio, you can simply download the project and open the "RinexReader.sln" file. If you would like to build it yourself, that is easy too. Create a new project and add the existing source and header files. 
Otherwise, it only makes use of features available in C++ standard library.

## Testing

To TEST the utility, you can check out "RinexReader.cpp" file which provides step-by-step instructions. I have also included sample Rinex Files (Observation and Navigation) in the "Input" Folder.

## License

Free-to-use (MIT), but at your own risk.
