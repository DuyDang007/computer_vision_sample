#include <vector>

class CameraIntrinsic
{
private:
    /* data */
public:
    int frame;
    int camId;
    float focalX;
    float focalY;
    float principleX;
    float principleY;
    CameraIntrinsic(int numargs, std::vector<std::string> valueVec);
    void printData();
};

class CameraExtrinsic
{
private:
    /* data */
public:
    int frame;
    int camId;
    float t1;
    float t2;
    float t3;
    float r11;
    float r12;
    float r13;
    float r21;
    float r22;
    float r23;
    float r31;
    float r32;
    float r33;
    CameraExtrinsic(int numargs, std::vector<std::string> valueVec);
    void printData();
};

template <typename T>
void readDataFile(std::string filePath, std::vector<T> &dataObjVect);