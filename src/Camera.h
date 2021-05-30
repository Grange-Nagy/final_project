/*
camera - class representing the camera viewport for drawing screen
*/

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <tuple>
#include <algorithm>

#include "Vector3.h"
#include "Object.h"

#define SUN_VECTOR 0.17609, 0.440225, 0.880451  //normalized

#define FOV_X 1.5708                //90 degrees in radians
#define FOV_Y 1.02974               //60 degrees in radians
#define SCREEN_ASPECT_RATIO 16/9    //shouldn't matter
#define FOCAL_LENGTH 1              //shouldn't matter, if really small causes floating point issues
#define TEXT_ASPECT_RATIO 0.5       //seems accurate for cmd.exe default font


//class representing the camera viewport for drawing screen
class Camera{
private:

    double width;
    double height;

    double cellWidth;
    double cellHeight;

    int resolution_x;
    int resolution_y;

    //CAMERA CAN ONLY LOOK DOWN THE Z AXIS BACKWARDS, mirrored x on if positive
    //this saves having to handle rotations of the camera
    Vector3 cameraOrigin;
    Vector3 cameraDirection;


    //ported from https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    bool isIntersecting(Vector3 rayVector,
                        Face    face){
        
        const double EPSILON = 0.0000001;
        Vector3 vertex0 = *(face.v1);
        Vector3 vertex1 = *(face.v2);  
        Vector3 vertex2 = *(face.v3);
        Vector3 edge1, edge2, h, s, q;
        double a,f,u,v;
        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;
        h = rayVector.crossProduct(edge2);
        a = edge1.dotProduct(h);
        if (a > -EPSILON && a < EPSILON)
            return false;    // This ray is parallel to this triangle.
        f = 1.0/a;
        s = cameraOrigin - vertex0;
        u = f * s.dotProduct(h);
        if (u < 0.0 || u > 1.0)
            return false;
        q = s.crossProduct(edge1);
        v = f * rayVector.dotProduct(q);
        if (v < 0.0 || u + v > 1.0)
            return false;
        // At this stage we can compute t to find out where the intersection point is on the line.
        // this doesn't matter too much and not doing it saves a dot product
        return true;
        /*
        double t = f * edge2.dotProduct(q);
        if (t > EPSILON) // ray intersection
        {
            //the intersection is rayOrigin + rayVector * t;
            return true;
        }
        else // This means that there is a line intersection but not a ray intersection.
            return true;
        */
        
    }


    //returns a character representing the brightness at a point (dot product of normal and global light)
    char getLuminosity(Face face){
        
        //both the normal and globalLight vectors should be normalized
        static Vector3 globalLight(SUN_VECTOR);

        //smaller character palette seems to look nicer at all font sizes
        std::string shades =  ".:-=+*#%@";
        
        double dotProduct = globalLight.dotProduct(*(face.norm));
        //if there is no illumination (i.e. back side)
        if(dotProduct < 0){
            return shades[0];
            
        }
        if(dotProduct > 1){
            //might happen if obj file is parsed wrong, normals need to be recalculated, or floating point rounding errors
            //std::cout << "Dot product in getLuminosity greater than 1" << std::endl;
            return shades[shades.size()-1];
        }
        
        //returns a char in the shades string based on the dot product
        return shades[(int)ceil(shades.size() * dotProduct) - 1];       //use ceil and -1 in case dot product is 1
    }   

    //get distances to triangles and sort them
    std::vector<std::tuple<double,int>> getDistances(Object obj){

        std::vector<Face> faceList = obj.getFaceList();

        //double is distance, int is index
        std::vector<std::tuple<double,int>> distances;

        //calulate distances for all faces
        for(int i = 0; i < faceList.size(); i++){
            //calculate distance and add to tuple with index
            distances.push_back(std::make_tuple(cameraOrigin.distance(faceList[i].center), i));
        }
        //sorts by the first element in the tuple, distance
        sort(distances.begin(), distances.end());
        return distances;
    }

    char drawRay(   Vector3 rayVector,
                    Object obj, 
                    std::vector<std::tuple<double,int>>* distances){
        
        std::vector<Face> faceList = obj.getFaceList();

        //for each face check if it intersects
        for(std::tuple<double,int> ele : *distances){

            if(isIntersecting(rayVector, faceList[std::get<1>(ele)])){
                //found a face
                return getLuminosity(faceList[std::get<1>(ele)]);
            }
        }
        //if no ray collides, return a space
        return ' ';
    }

public:
    //constructor initalizes the drawing plane
    Camera(int resolution_x, Vector3 cameraOrigin, Vector3 cameraDirection):
        cameraOrigin(cameraOrigin), cameraDirection(cameraDirection),resolution_x(resolution_x){
        //define a drawing plane made of cells that have the aspect ratio of text
        //drawing plane lives on x/y plane, camera looking down the z axis backwards
        //should have cell dimension in x of resolution_x and be FOCAL_LENGTH away

        //get plane width and height
        width = tan(FOV_X/2)*FOCAL_LENGTH;
        height = tan(FOV_Y/2)*FOCAL_LENGTH;

        //calculate cell size
        cellWidth = width/resolution_x;
        cellHeight = cellWidth / TEXT_ASPECT_RATIO;

        resolution_y = floor(height/cellHeight);
    }
    //default destuctor
    ~Camera(){}
    
    //render the given object as viewed through this camera
    std::string renderObject(Object object){
        //save a copy of the frame to return for animation
        std::stringstream frame;

        //this will be the point in space to which the vectors are drawn from the origin to the plane
        Vector3 drawPoint = cameraOrigin + (cameraDirection * FOCAL_LENGTH); //center
        //move to top left corner
        drawPoint.x -= width/2;
        drawPoint.y += height/2;
        
        double leftX = drawPoint.x;     //save the leftmost x coord

        //calcuate face distances and keep them cached
        std::vector<std::tuple<double,int>> distances = getDistances(object);

        //walk the drawing plane firing rays
        for(int y = 0; y < resolution_y; y++){
            for(int x = 0; x < resolution_x; x++){
                //draw ray from camera to the drawPoint on the plane
                Vector3 ray = cameraOrigin.vectorTo(drawPoint);
                char temp = drawRay(ray,object,&distances);
                std::cout << temp;
                frame << temp;
                //move the drawing point to the right
                drawPoint.x += cellWidth;
            }
            //add newline and move pointer back to the left and lower it
            std::cout << std::endl;
            frame << '\n';
            drawPoint.x = leftX;
            drawPoint.y -= cellHeight;
        }
        
        return frame.str();
    }


};