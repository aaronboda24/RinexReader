# RinexReader

RINEX is a standardazied file format which is used to store raw GNSS observation data. This program supports RINEX v2.x (GPS only) and RINEX v3.x (GPS, GLONASS, GALILEO). You can find more information [here](https://kb.igs.org/hc/en-us/articles/115003980188-RINEX-2-11).

## Motivation

Making a RINEX Reader often becomes a redundant and time-consuming part for someone new starting out with GNSS processing. This utility will help one read and organize the observation files to use for their processing. 

## Installation

If you have Visual Studio, you can simply download the project and open the "RinexReader.sln" file. If you would like to build it yourself, that is easy too. Create a new project and add the existing source and header files. Otherwise, it only makes use of features available in C++ standard library.

## Testing

For instructions on how to use this program, you can check out "RinexReader.cpp". I have also included sample RINEX v2.x and v3.x files (Observation and Navigation) in the "Input" Folder.

## Reporting Bugs

This is a work in progress. Kindly report any bugs to aaronboda.gh@mail.com along with the observation files and a brief explanation on how to recreate the problem.

## License

Free-to-use (MIT), but at your own risk.
