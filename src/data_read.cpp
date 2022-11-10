/* Read data in a text file and represent as a struct
param1 param2 param3 ..
value1 value2 value3 ..
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdarg.h>

#include "data_read.hpp"

/* Read a text file and return result to each element of struct array */

using namespace std;

/******************************************************************************
 * Camera intrinsic class members
******************************************************************************/
CameraIntrinsic::CameraIntrinsic(int numargs, vector<string> valueVec)
{
    if (numargs != 6)
    {
        cerr << "Invalid number of intrinsic param" << endl;
    }
    frame = atoi(&valueVec.at(0)[0]); // Address of element 0
    camId = atoi(&valueVec.at(1)[0]);
    focalX = atof(&valueVec.at(2)[0]);
    focalY = atof(&valueVec.at(3)[0]);
    principleX = atof(&valueVec.at(4)[0]);
    principleY = atof(&valueVec.at(5)[0]);
}

void CameraIntrinsic::printData()
{
    cout << frame << " | " << camId << " | " << focalX << " | " << focalY << endl;
}

/******************************************************************************
 * Camera extrinsic class members
******************************************************************************/


/* Read data from input file and parse to object vector of type T
Make sure class T has a public method readFromData
*/
template <typename T>
void readDataFile(string filePath, vector<T> &dataObjVect)
{
    string lineStr;
    string valueStr;

    ifstream dataFile(filePath);
    if (!dataFile)
    {
        cerr << "Cannot open file: " << filePath << endl;
        return;
    }

    // Always skip 1st line
    getline(dataFile, lineStr);

    // Get a line from stream
    while (getline(dataFile, lineStr))
    {
        // Create a stringstream object from the line
        stringstream lineStream(lineStr);
        // Extract every value from line string stream
        vector<string> valueVect;
        while (getline(lineStream, valueStr, ' '))
        {
            valueVect.push_back(valueStr);
        }

        T dataObj(valueVect.size(), valueVect);

        dataObjVect.push_back(dataObj);
    }
}

/* Test part */
int main(int argc, char *argv[])
{
    vector<CameraIntrinsic> camVec;
    readDataFile("../../VKITTI2/Scene18/morning/intrinsic.txt", camVec);

    for(int i = 0; i < camVec.size(); i++)
    {
        camVec.at(i).printData();
    }
    return 0;
}