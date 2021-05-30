# Command Line Raytracer

Raytracer capable of rendering stills and animations of triangularized waveform .obj files on the command line  
Should compile/run on Windows/Unix

## Usage
.obj file needs to be triangularized with good normals  
Works much better on a console with a smaller font with aspect ratio close to 0.5

### Compile on Windows or Unix
```plaintext
g++ main.cpp -Ofast -o CLIraytracer
```

### Run
```plaintext
./CLIraytracer OBJ_FILEPATH [-h,--help] [-r, --resolution=WIDTH] [-n, --no-update-normals]
     [-a, --animate=?FRAMES ROTATIONS] [-d, --distance=DISTANCE] 
```