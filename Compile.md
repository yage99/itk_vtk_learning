# Compilation

## Compile Itk

### Compile opencv first

* version: 3.2-2.0

create make directory and `CMAKE_CXX_FLAGS=-std=c++11 cmake ..` then `make` and `make install` openCV to you system.

### Compile vtk

### Compile itk

itk should be compiled with option `Module_ITKVideoBridgeOpenCV` and `Module_ITKVtkGlue`:
`CMAKE_CXX_FLAGS=-std=c++11 cmake -DModule_ITKVideoBridgeOpenCV:BOOL=ON -DModule_ITKVtkGlue:BOOL=ON ..
`