/*
Vector3 - first principlas implementation of the common Vector3 struct
*/


#pragma once

#include <sstream>
#include <math.h>

struct Vector3{

    double x,y,z;

    //PROJECT: Polymorphism
    //default
    Vector3(){}

    //normal constructor
    Vector3(double x, double y, double z):x(x),y(y),z(z){}

    //from sstream
    Vector3(std::stringstream& line){
        //0.0000 1.0000 0.0000
        line >> x >> y >> z;
    }

    //PROJECT: Operator Overloading
    Vector3 operator+(const Vector3& toAdd){
        return Vector3( x + toAdd.x,
                        y + toAdd.y,
                        z + toAdd.z);
    }

    Vector3 operator-(const Vector3& toSub){
        return Vector3( x - toSub.x,
                        y - toSub.y,
                        z - toSub.z);
    }

    Vector3 operator*(const double& toMulti){
        return Vector3( x * toMulti,
                        y * toMulti,
                        z * toMulti);
    }

    Vector3 crossProduct(Vector3 b){
        return Vector3( (y * b.z) - (z * b.y),
                        (z * b.x) - (x * b.z),
                        (x * b.y) - (y * b.x));
    }

    double dotProduct(Vector3 b){
        return (x * b.x) + (y * b.y) + (z * b.z);
    }

    double distance(Vector3 b){
        return sqrt(pow((x - b.x),2) +
                    pow((y - b.y),2) +
                    pow((z - b.z),2));
    }

    //same as b - this
    Vector3 vectorTo(Vector3 b){
        return Vector3( b.x - x,
                        b.y - y,
                        b.z - z);

    }

    //rotate 3d point or vector about Y using hardcoded rotation matrix math
    void rotateAboutY(double radians){
        double tempX = (x * cos(radians)) + (z * sin(radians));
        double tempY = y;
        double tempZ = (-x * sin(radians)) + (z * cos(radians));

        x = tempX;
        y = tempY;
        z = tempZ;
        return;
    }

};