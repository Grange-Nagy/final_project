/*
raytracer entrypoint - parse commandline arguments and handle animation
*/


#include <iostream>
#include <string>
#include <time.h>
#include <sstream>
#include <exception>
#include "Vector3.h"
#include "Object.h"
#include "Camera.h"


#ifdef _WIN32
#include <windows.h>
#endif

#define PI 3.14159265358979323846

#define USAGE_STRING "Usage:\n./execute OBJ_FILEPATH [-h,--help] [-r, --resolution=WIDTH] [-n, --no-update-normals]\n\t[-a, --animate=?FRAMES ROTATIONS] [-d, --distance=DISTANCE]"



//portable windows build with g++ main.cpp -static-libstdc++ -static-libgcc -Ofast -o CLIraytracer
//normal g++ main.cpp -Ofast -o CLIraytracer
int main(int argc, char** argv){

    //check if enough arguments
    if(argc < 2 || argv[1][0] == '-'){
        std::cout << USAGE_STRING << std::endl;
        return 1;
    }

    std::string filepath(argv[1]);

    int x_resolution = 100;
    bool isAnimated = false;
    int frameCount = 12;
    double rotations_per_second = 0.5;
    int distance = 10;
    bool updateNormals = true;

    //parse optional command line arguments
    for(int i=2; i<argc; i++){
        std::string token(argv[i]);

        if(token == "-h" || token == "--help"){
            std::cout << USAGE_STRING << std::endl;
            return 1;
        }

        if(token == "-r" || token == "--resolution"){
            x_resolution = std::stoi(argv[i+1]);
            i++;    //skip over arg
            continue;
        }
        if(token == "-a" || token == "--animate"){
            isAnimated = true;
            
            //check if optional args set, make sure not at the end, these need to be evaluated in order
            if(i+2 < argc && argv[i+1][0] != '-'){
                frameCount = std::stoi(argv[i+1]);
                rotations_per_second = std::stod(argv[i+2]);
                i+=2;       //skip over args
            }
            
            continue;
        }

        if(token == "-d" || token == "--distance"){
            distance = std::stoi(argv[i+1]);
            i++;        //skip over arg
            continue;
        }
        if(token == "-n" || token == "--no-update-normals"){
            updateNormals = false;
            continue;
        }

    }
    //PROJECT: Exceptions
    //load object
    Object object;
    try{
        object.loadFile(filepath);
    }catch(std::runtime_error &e){
        std::cout << e.what() << std::endl;
        std::cout << "Exiting..." << std::endl;
        return -1;
    }
    
    Vector3 cameraOrigin(0,0,distance);
    //camera needs to be looking down the Z axis
    Vector3 cameraDirection(0,0,-1);
    Camera camera(x_resolution, cameraOrigin,cameraDirection);

    //store each frame as a string for animation
    std::string* frames = new std::string[frameCount];

    std::cout << std::endl;     //extra newline
    
    //only render one frame if not animated
    if(!isAnimated){
        frameCount = 1;
    }
    

    //rotate the object by 2pi/FRAMES per render
    for(int i = 0; i < frameCount; i++){
        frames[i] = camera.renderObject(object);
        object.rotateAboutY((2*PI)/frameCount, updateNormals);
    }

    //exit application after render if not animated
    if(!isAnimated){
        return 0;
    }

    //get the sleep time per frame in ms
    int msFrameTime = (1000/rotations_per_second)/frameCount;
    #ifdef _WIN32
    //animation loop does not clear on windows
    //clearing the console on windows spawns a new process and takes ~50ms
    system("cls");
    #endif
    int i = 0;
    //needs to be exited with a keyboard interrupt while animating
    while(true){

        //start the clock before the print so that frame times are not dependent on i/o speed
        clock_t frame_end;
        frame_end = clock() + msFrameTime * CLOCKS_PER_SEC/1000;   
        std::cout << "Ctrl+C to exit" << std::endl;                 //don't want to write cross platform keyboard input detection
        std::cout << frames[i]; //print the frame
        
        #ifdef _WIN32
        //on windows move the cursor instead of clearing to prevent flickering because of slow windows console i/o
        static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);     //get the console handle
        std::cout.flush();
        static const COORD coord = { (SHORT)0, (SHORT)0};   //coordinates at top left of console
        SetConsoleCursorPosition(hOut, coord);
        #else
        //clears the console on unix systems with escape codes
        std::cout << "\e[1;1H\e[2J";
        #endif      

        while (clock() < frame_end){}    //this locks the thread
        
        i = (i+1)%frameCount;   //loops animation
    }

    
    return 0;
}