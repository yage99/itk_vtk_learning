CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET( PROJECT_NAME ITK_Tutorial_ML )

# Set project name 
PROJECT( ${PROJECT_NAME} )
 
#Find libraries
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# ITKVtkGlue is for visualization only; this part is required if you have build ITK with VTK support
# If you have build ITK without VTK support, please delete the following IF{} loop 
IF( ITKVtkGlue_LOADED )
  FIND_PACKAGE(VTK REQUIRED)
  INCLUDE(${VTK_USE_FILE})
ELSE()
  FIND_PACKAGE(ItkVtkGlue REQUIRED)
  INCLUDE(${ItkVtkGlue_USE_FILE})
  SET(Glue ItkVtkGlue)
ENDIF()

# Add sources to executable
ADD_EXECUTABLE(
  ${PROJECT_NAME} 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cxx
)

# Link the libraries to be used
IF( ITKVtkGlue_LOADED )
  TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
  	${Glue}  
    ${VTK_LIBRARIES} 
    ${ITK_LIBRARIES}
    ${OpenCV_LIBS}
  )
ELSE()
  TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    ${ITK_LIBRARIES}
    ${OpenCV_LIBS}
  )
ENDIF()
