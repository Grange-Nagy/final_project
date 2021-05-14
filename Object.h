#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "Vector3.h"

#define NAME_TOKEN      "o"
#define VECTOR_TOKEN    "v"
#define NORMAL_TOKEN    "vn"
#define FACE_TOKEN      "f"

struct Face{

    //pointers to vertex and normals
    Vector3* v1;
    Vector3* v2;
    Vector3* v3;
    Vector3* norm;

    //centroid of triangle
    Vector3 center;

    //simple constructor, calculating centroid
    Face(Vector3* v1, Vector3* v2, Vector3* v3, Vector3* norm):
        v1(v1),v2(v2),v3(v3),norm(norm),center(Vector3( (v1->x + v2->x + v3->x) / 3,
                                                        (v1->y + v2->y + v3->y) / 3,
                                                        (v1->z + v2->z + v3->z) / 3)){

    }


};

class Object{

private:
    std::string name;
    std::vector<Vector3> vertexList;
    std::vector<Vector3> normalList;
    std::vector<Face> faceList;



public:

    int numFaces;

    //create object from .obj file with trianglarized mesh
    Object (std::string filepath){
        
        std::ifstream objFile;
        objFile.open(filepath);
        
        std::string line;
        std::string first_token;

        while(std::getline(objFile, line)){
            
            std::stringstream lineStream(line);
            lineStream >> first_token;

            //parse lines based on leading token

            //o object
            if(first_token == NAME_TOKEN){
                
                lineStream >> name;
                continue;
            }

            //v 1.000000 1.000000 -1.000000
            if(first_token == VECTOR_TOKEN){
                vertexList.push_back(Vector3(lineStream));
                continue;
            }
            //vn 0.0000 1.0000 0.0000
            if(first_token == NORMAL_TOKEN){
                normalList.push_back(Vector3(lineStream));
                continue;
            }
            //f 5/1/1 3/2/1 1/3/1
            if(first_token == FACE_TOKEN){
                //parse face
                std::string temp;
                lineStream >> temp;
                std::stringstream token1(temp);

                std::string value;
                std::getline(token1,value,'/');
                //this will be the first vertex
                int v1 = std::stoi(value);
                std::getline(token1,value,'/');
                //ignore texture channel
                std::getline(token1,value,' ');
                //get normal
                int norm = std::stoi(value);

                lineStream >> temp;
                std::stringstream token2(temp);
                 std::getline(token2,value,'/');
                //this will be the second vertex
                int v2 = std::stoi(value);

                lineStream >> temp;
                std::stringstream token3(temp);
                 std::getline(token3,value,'/');
                //this will be the third vertex
                int v3 = std::stoi(value);

                //add vertices and normals by reference
                faceList.push_back(Face(&vertexList[v1-1],  //one indexed
                                        &vertexList[v2-1],
                                        &vertexList[v3-1],
                                        &normalList[norm]));
                continue;
            }

        }

        numFaces = faceList.size();
        objFile.close();

    }

    //read only access
    std::vector<Face> getFaceList(){
        return faceList;
    }
};