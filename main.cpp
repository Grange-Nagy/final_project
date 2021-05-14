#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <tuple>
#include <algorithm>

#include "Vector3.h"
#include "Object.h"

#define SUN_VECTOR 0.17609, 0.440225, 0.880451  //normalized

#define RESOLUTION_X 800
#define FOV_X 1.5708                //90 degrees in radians
#define FOV_Y 1.7708                //90 degrees in radians
//#define FOV_Y 1.02974               //60 degrees in radians
#define SCREEN_ASPECT_RATIO 16/9    //shouldn't matter
#define FOCAL_LENGTH 3              //shouldn't matter
#define TEXT_ASPECT_RATIO 0.5       //seems accurate for cmd.exe default font



//ported from https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool isIntersecting(Vector3 rayOrigin,
                    Vector3 rayVector,
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
    s = rayOrigin - vertex0;
    u = f * s.dotProduct(h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = s.crossProduct(edge1);
    v = f * rayVector.dotProduct(q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    // this doesn't matter too much and not doing it saves a dot product
    #ifndef HANDLE_EDGES
    return true;
    #endif
    #ifdef HANDLE_EDGES
    double t = f * edge2.dotProduct(q);
    if (t > EPSILON) // ray intersection
    {
        //the intersection is rayOrigin + rayVector * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return true;
    #endif
    
}



char getLuminosity(Face face){
    
    //both the normal and globalLight vectors should be normalized
    Vector3 globalLight(SUN_VECTOR);

    #ifndef MORE_SHADES
    std::string shades =  ".:-=+*#%@";
    #endif
    #ifdef MORE_SHADES
    std::string shades = "`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    #endif
    
    double dotProduct = globalLight.dotProduct(*(face.norm));
    //if there is no illumination (i.e. back side)
    if(dotProduct < 0){
        return shades[0];
        
    }
    if(dotProduct > 1){
        //might happen if obj file is parsed wrong or of normals need to be recalculated
        std::cout << "Dot product in getLuminosity greater than 1" << std::endl;
    }
    
    //returns a char in the shades string based on the dot product
    return shades[(int)ceil(shades.size() * dotProduct) - 1];       //use ceil and -1 in case dot product is 1
}   

//get distances to triangles and sort them
std::vector<std::tuple<double,int>> getDistances(Vector3 rayOrigin,Object obj){

    std::vector<Face> faceList = obj.getFaceList();

    //double is distance, int is index
    std::vector<std::tuple<double,int>> distances;

    //calulate distances for all faces
    for(int i = 0; i < faceList.size(); i++){
        //calculate distance and add to tuple with index
        distances.push_back(std::make_tuple(rayOrigin.distance(faceList[i].center), i));
    }
    //sorts by the first element in the tuple, distance
    sort(distances.begin(), distances.end());
    return distances;
}

char drawRay(   Vector3 rayOrigin, 
                Vector3 rayVector,
                Object obj, 
                std::vector<std::tuple<double,int>>* distances){
    
    std::vector<Face> faceList = obj.getFaceList();

    //for each face check if it intersects
    for(std::tuple<double,int> ele : *distances){

        if(isIntersecting(rayOrigin, rayVector, faceList[std::get<1>(ele)])){
            //found a face
            return getLuminosity(faceList[std::get<1>(ele)]);
        }
    }
    //if no ray collides, return a space
    return ' ';
}





int main(){

    Object object("kermit.obj");
    
    Vector3 cameraOrigin(0,0,20);

    //CAMERA CAN ONLY LOOK DOWN THE Z AXIS BACKWARDS, mirrored x on if positive
    //this saves having to handle rotations of the camera
    Vector3 cameraDirection(0,0,-1);


    //define a drawing plane made of cells that have the aspect ratio of text
    //drawing plane lives on x/y plane, camera looking down the z axis backwards
    //should have cell dimensions RESOLUTION_X and be FOCAL_LENGTH away

    //get plane width and height
    double width = tan(FOV_X/2)*FOCAL_LENGTH;
    double height = tan(FOV_Y/2)*FOCAL_LENGTH;

    //calculate cell size
    double cellWidth = width/RESOLUTION_X;
    double cellHeight = cellWidth / TEXT_ASPECT_RATIO;

    int resolution_y = floor(height/cellHeight);

    //this will be the point in space to which the vectors are drawn from the origin to the plane
    Vector3 drawPoint = cameraOrigin + (cameraDirection * FOCAL_LENGTH); //center
    //move to top left corner
    drawPoint.x -= width/2;
    drawPoint.y += height/2;

    double leftX = drawPoint.x;     //save the leftmost x coord

    //calcuate face distances and keep them cached
    std::vector<std::tuple<double,int>> distances = getDistances(cameraOrigin,object);

    //walk the drawing plane firing rays
    for(int y = 0; y < resolution_y; y++){
        for(int x = 0; x < RESOLUTION_X; x++){
            //draw ray from camera to the drawPoint on the plane
            Vector3 ray = cameraOrigin.vectorTo(drawPoint);
            std::cout << drawRay(cameraOrigin,ray,object,&distances);
            //move the drawing point to the right
            drawPoint.x += cellWidth;
        }
        //add newline and move pointer back to the left and lower it
        std::cout << std::endl;
        drawPoint.x = leftX;
        drawPoint.y -= cellHeight;
    }

    return 0;
}