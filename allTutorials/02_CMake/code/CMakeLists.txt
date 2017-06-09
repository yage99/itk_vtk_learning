CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Assign a separate variable for project name
SET( PROJECT_NAME CMakeTutorial )

# Change this if you want different executable name
SET( EXE_NAME ${PROJECT_NAME} )

# Add definitions to use inside source code
ADD_DEFINITIONS( -DPROJECT_NAME="${PROJECT_NAME}" )

# Set project name 
PROJECT( ${PROJECT_NAME} )

# Add all include directories 
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src )

# Add sources to executable
ADD_EXECUTABLE( ${EXE_NAME} ${PROJECT_SOURCE_DIR}/src/main.cxx)
