#include <stdio.h>
#include <math.h>
#include <string>
using namespace std;

#define NUMBER_OF_COLORS 256
#define BYTES_PER_COLOR 3

int convertPALtoHSP( char * filePath )
{
	//open the file
	FILE * file;
	if(int error = fopen_s(&file, filePath, "r") != 0)
	{
		return error; //couldn't open the file
	}

	//check the header
	char * header1 = new char[100];
	char * header2 = new char[100];
	char * header3 = new char[100];

	if(fgets(header1, 100, file) == NULL) { return -1; }
	if(fgets(header2, 100, file) == NULL) { return -1; }
	if(fgets(header3, 100, file) == NULL) { return -1; }

	if(strcmp(header1, "JASC-PAL\n") != 0
		|| strcmp(header2, "0100\n") != 0
		|| strcmp(header3, "256\n") != 0)
	{
		return -1; //improper header
	}

	delete[] header1;
	delete[] header2;
	delete[] header3;

	//convert the color value strings to actual data and put it all in an output buffer
	char * color = new char[100];
	string colorStr;
	string redStr;
	string greenStr;
	string blueStr;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	char digit;
	unsigned int firstSpace;
	unsigned int lastSpace;
	unsigned char * output = new unsigned char[NUMBER_OF_COLORS * BYTES_PER_COLOR];
	for(int i = 0; i < NUMBER_OF_COLORS * BYTES_PER_COLOR; i += BYTES_PER_COLOR)
	{
		//get a line from the document and store it as a string
		if(fgets(color, 100, file) == NULL) { return -1; }
		colorStr = color;

		//find where the spaces are
		firstSpace = colorStr.find_first_of(" ");
		lastSpace = colorStr.find_last_of(" ");

		//extract the individual colors
		redStr = colorStr.substr(0, firstSpace);
		greenStr = colorStr.substr(firstSpace + 1, lastSpace - (firstSpace + 1));
		blueStr = colorStr.substr(lastSpace + 1, colorStr.length() - 1 - (lastSpace + 1));

		//convert the red string to an actual number
		red = 0;
		for(unsigned int j = 0; j < redStr.length(); j++)
		{
			//get a digit as a char
			redStr.copy(&digit, 1, j);
			//subtract 48 (the ascii index of the char '0') from it to get the real number
			digit -= 48;

			//add the digit, multiplied by the proper power of 10
			red = red + (digit * pow(10.0, (int)(redStr.length() - j - 1)));
		}
		//convert the green string to an actual number
		green = 0;
		for(unsigned int j = 0; j < greenStr.length(); j++)
		{
			//get a digit as a char
			greenStr.copy(&digit, 1, j);
			//subtract 48 (the ascii index of the char '0') from it to get the real number
			digit -= 48;

			//add the digit, multiplied by the proper power of 10
			green = green + (digit * pow(10.0, (int)(greenStr.length() - j - 1)));
		}
		//convert the blue string to an actual number
		blue = 0;
		for(unsigned int j = 0; j < blueStr.length(); j++)
		{
			//get a digit as a char
			blueStr.copy(&digit, 1, j);
			//subtract 48 (the ascii index of the char '0') from it to get the real number
			digit -= 48;

			//add the digit, multiplied by the proper power of 10
			blue = blue + (digit * pow(10.0, (int)(blueStr.length() - j - 1)));
		}

		//put all these values into the output buffer, in BGR order
		output[i] = blue;
		output[i+1] = green;
		output[i+2] = red;
	}

	delete[] color;

	fclose(file);

	//create the name of the output file
	string inName = filePath;
	string outName;
	unsigned int dotPos = inName.find_first_of(".");
	outName = inName.substr(0, dotPos) + ".hsp";

	//create an output file
	FILE * outFile;
	if(int error = fopen_s(&outFile, outName.data(), "wb") != 0)
	{
		return error; //couldn't create the file
	}

	//put the output buffer data into the output file
	if(fwrite(output, BYTES_PER_COLOR, NUMBER_OF_COLORS, outFile) != NUMBER_OF_COLORS)
	{
		return -1; //there was an error in writing
	}

	//close the file
	fclose(outFile);

	//delete the output
	delete[] output;

	return 0;
}

int main( int argc, char* argv[] )
{
	int error;
	if(argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{
			error = convertPALtoHSP(argv[i]);
		}
	}
	//convertPALtoHSP("john\\john_01.pal");

	return 0;
}