#include <iostream>
#include <string>
#include <time.h>
#include "Vector3.h"
#include "Object.h"
#include "Camera.h"


#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#define PI 3.14159265358979323846
#define RPS 0.5
#define FRAMES 48

#ifdef _WIN32
void setCursorPosition(int x, int y)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}
#endif

int main(){
    Object object("kirby.obj");
    
    Vector3 cameraOrigin(0,0,4);
    Vector3 cameraDirection(0,0,-1);
    Camera camera(300, cameraOrigin,cameraDirection);

    //store each frame as a string for animation
    std::string frames[FRAMES];

    //rotate the object by 2pi/FRAMES per render
    for(int i = 0; i < FRAMES; i++){
        frames[i] = camera.renderObject(object);
        object.rotateAboutY((2*PI)/FRAMES, true);
    }

    //get the sleep time per frame in ms
    int msFrameTime = (1000/RPS)/FRAMES;
    #ifdef _WIN32
    //animation loop does not clear on windows
    system("cls");
    #endif
    int i = 0;
    //needs to be exited with a keyboard interrupt 
    while(true){
        //print the frame

        
        //start the clock before the print so that frame times are not dependent on i/o speed
        clock_t time_end;
        time_end = clock() + msFrameTime * CLOCKS_PER_SEC/1000;   
        std::cout << frames[i];
        
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