# Compilation

## QT

Qt is used through out vtk and itk and Captk.
So a right qt is reuqired to be installed in advance.

`./Configure` to configure qt
`gmake` to make qt
and `gmake install` to install qt to your system.

## Compile Itk

### Compile opencv first

* version: 3.2-2.0

create make directory and using command:
`CMAKE_CXX_FLAGS=-std=c++11 cmake ..` then `make` and `make install`
to compile and install openCV to you system.

### Compile vtk

### Compile itk

itk should be compiled with option `Module_ITKVideoBridgeOpenCV` and `Module_ITKVtkGlue`:
`CMAKE_CXX_FLAGS=-std=c++11 cmake -DModule_ITKVideoBridgeOpenCV:BOOL=ON -DModule_ITKVtkGlue:BOOL=ON ..`
