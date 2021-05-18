# CS151 Final Project - Raytracer

Raytracer capable of rendering stills and animations of triangularized waveform .obj files on the command line
Should work on Windows/Unix

## Usage
.obj file needs to be triangularized with good normals  
Works much better on a console with a smaller font with aspect ratio close to 0.5

### Compile on Windows or Unix
```plaintext
g++ main.cpp -Ofast -o execute
```

### Run
```plaintext
./execute OBJ_FILEPATH [-h,--help] [-r, --resolution=OUTPUT_CHAR_WIDTH] [-n, --no-update-normals]
     [-a, --animate=?FRAMES_TO_RENDER ROTATIONS_PER_SECOND] [-d, --distance=CAMERA_DISTANCE_FROM_ORIGIN] 
```


## License
[MIT](https://choosealicense.com/licenses/mit/)