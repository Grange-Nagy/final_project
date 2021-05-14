#pragma once

#include <sstream>

struct Vector3{

    double x,y,z;

    //default
    Vector3(){}

    //normal constructor
    Vector3(double x, double y, double z):x(x),y(y),z(z){}

    //from sstream
    Vector3(std::stringstream& line){
        //0.0000 1.0000 0.0000
        line >> x >> y >> z;
    }

    Vector3 operator+(const Vector3& toAdd){
        return Vector3( this->x + toAdd.x,
                        this->y + toAdd.y,
                        this->z + toAdd.z);
    }

    Vector3 operator-(const Vector3& toSub){
        return Vector3( this->x - toSub.x,
                        this->y - toSub.y,
                        this->z - toSub.z);
    }

    Vector3 operator*(const double& toMulti){
        return Vector3( this->x * toMulti,
                        this->y * toMulti,
                        this->z * toMulti);
    }

    Vector3 crossProduct(Vector3 b){
        return Vector3( (this->y * b.z) - (this->z * b.y),
                        (this->z * b.x) - (this->x * b.z),
                        (this->x * b.y) - (this->y * b.x));
    }

    double dotProduct(Vector3 b){
        return (this->x * b.x) + (this->y * b.y) + (this->z * b.z);
    }

    double distance(Vector3 b){
        return sqrt(pow((this->x - b.x),2) +
                    pow((this->y - b.y),2) +
                    pow((this->z - b.z),2));
    }

    Vector3 vectorTo(Vector3 b){
        return Vector3( b.x - this->x,
                        b.y - this->y,
                        b.z - this->z);

    }

};