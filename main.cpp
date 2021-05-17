#include <iostream>
#include <string>
#include <time.h>
#include "Vector3.h"
#include "Object.h"
#include "Camera.h"


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

#define PI 3.14159265358979323846
#define ROTATIONS_PER_SECOND 0.5
#define FRAMES 120

#ifdef _WIN32
//windows only function to move the console cursor
void setCursorPosition(int x, int y)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}
#endif
/*
TODO: add commandline args for x resolution -r 800,
animation frames and rotations per second   -a 24 0.5,
                         distance from obj  -d 4,
            if normals shouldn't be updated -n

*/
//build with g++ main.cpp -Ofast -o optimized
int main(){
    Object object("mole.obj");
    
    Vector3 cameraOrigin(0,0,100);
    Vector3 cameraDirection(0,0,-1);
    Camera camera(800, cameraOrigin,cameraDirection);

    //store each frame as a string for animation
    std::string frames[FRAMES];

    std::cout << std::endl;     //extra newline

    //rotate the object by 2pi/FRAMES per render
    for(int i = 0; i < FRAMES; i++){
        frames[i] = camera.renderObject(object);
        object.rotateAboutY((2*PI)/FRAMES, true);
    }

    //get the sleep time per frame in ms
    int msFrameTime = (1000/ROTATIONS_PER_SECOND)/FRAMES;
    #ifdef _WIN32
    //animation loop does not clear on windows
    system("cls");
    #endif
    int i = 0;
    //needs to be exited with a keyboard interrupt 
    while(true){

        //start the clock before the print so that frame times are not dependent on i/o speed
        clock_t time_end;
        time_end = clock() + msFrameTime * CLOCKS_PER_SEC/1000;   

        std::cout << frames[i]; //print the frame
        
        #ifdef _WIN32
        //on windows move the cursor instead of clearing to prevent flickering because of insanely slow cmd i/o
        setCursorPosition(0,0);
        #else
        system("clear");
        #endif      

        while (clock() < time_end){}    //this locks the thread
        
        i = (i+1)%FRAMES;   //loops animation
    }

    #ifdef _WIN32
    //animation loop does not clear on windows
    system("cls");
    #endif

    
    return 0;
}