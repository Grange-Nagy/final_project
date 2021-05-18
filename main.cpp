#include <iostream>
#include <string>
#include <time.h>
#include <sstream>
#include "Vector3.h"
#include "Object.h"
#include "Camera.h"


#ifdef _WIN32
#include <windows.h>
#endif

#define PI 3.14159265358979323846

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




//build with g++ main.cpp -static-libstdc++ -static-libgcc -Ofast -o CLIraytracer
int main(int argc, char** argv){

    if(argc < 2 || argv[1][0] == '-'){
        std::cerr <<"Usage: " << argv[0] << " <obj file> (needs to be triangularized with good normals)\n"
                    "Optional commandline args:\n"
                    "This menu                                                  -h/-u or --help/--usage\n"
                    "For x resolution                                           -r or --resolution <int>\n"
                    "If animation, (optional) frames and rotations per second   -a or --animate ?<int> <double>\n"
                    "Distance from obj                                          -d or --distance <int>\n"
                    "If normals shouldn't be updated                            -n or --dontupdatenormals" << std::endl;
        return 1;
    }

    std::string filepath(argv[1]);

    int x_resolution = 100;
    bool isAnimated = false;
    int frameCount = 12;
    double rotations_per_second = 0.5;
    int distance = 10;
    bool updateNormals = true;

    for(int i=2; i<argc; i++){
        std::string token(argv[i]);

        if(token == "-u" || token == "-h" || token == "--help" || token == "-usage"){
            std::cerr <<"Usage: " << argv[0] << " <obj file> (needs to be triangularized with good normals)\n"
                        "Works much better with a smaller font with aspect ratio close to 0.5:\n"
                        "Optional commandline args:\n"
                        "This menu                                                  -h/-u or --help/--usage\n"
                        "For x resolution                                           -r or --resolution <int>\n"
                        "If animation, (optional) frames and rotations per second   -a or --animate ?<int> <double>\n"
                        "Distance from obj                                          -d or --distance <int>\n"
                        "If normals shouldn't be updated                            -n or --dontupdatenormals" << std::endl;
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
        if(token == "-n" || token == "--dontupdatenormals"){
            updateNormals = false;
            continue;
        }

    }
    
    Object object(filepath);
    
    Vector3 cameraOrigin(0,0,distance);
    Vector3 cameraDirection(0,0,-1);
    Camera camera(x_resolution, cameraOrigin,cameraDirection);

    //store each frame as a string for animation
    std::string* frames = new std::string[frameCount];

    std::cout << std::endl;     //extra newline
    
    //only render one fram if not animated
    if(!isAnimated){
        frameCount = 1;
    }
    

    //rotate the object by 2pi/FRAMES per render
    for(int i = 0; i < frameCount; i++){
        frames[i] = camera.renderObject(object);
        object.rotateAboutY((2*PI)/frameCount, updateNormals);
    }

    //exit application if not animated
    if(!isAnimated){
        return 0;
    }

    //get the sleep time per frame in ms
    int msFrameTime = (1000/rotations_per_second)/frameCount;
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
        //clears the console on unix systems
        std::cout << "\e[1;1H\e[2J";
        #endif      

        while (clock() < time_end){}    //this locks the thread
        
        i = (i+1)%frameCount;   //loops animation
    }

    #ifdef _WIN32
    //animation loop does not clear on windows
    system("cls");
    #endif

    
    return 0;
}