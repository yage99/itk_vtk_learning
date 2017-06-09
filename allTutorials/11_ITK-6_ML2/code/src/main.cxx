/**
\brief 11_ITK-ML-2: Advanced machine learning example
*/
#include <vector>
#include <string>
#include <algorithm>
#include <tuple>

//! ITK headers
#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageIOBase.h"
#include "itkImageIOFactory.h"
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"

#include "itkMaskImageFilter.h"
#include "itkImageToListSampleFilter.h"


#include "itkOpenCVImageBridge.h"

#include "vnl/vnl_matrix.h"
#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "cbicaUtilities.h"

#define ROWS 4
#define COLS 2

/**
\brief Get the itk::Image

\param The itk::Image which will contain the image data
\param File name of the image
*/
template <class TImageType>
void SafeReadImage(typename TImageType::Pointer image, const std::string &fName)
{
  typedef TImageType ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(fName);

  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject& e)
  {
    std::cerr << "Exception caught: " << e.what() << "\n";
    return;
  }

  image->Graft(reader->GetOutput());
  return;
}

/**
\brief Splits the input file name into its constituents

\param dataFile The full file name which is the input
\param baseName Overwritten with file name without extension
\param extension Overwritten with extension without '.'
\param path Overwritten with path to file

\return True if successful
*/
bool splitFileName( const std::string &dataFile, std::string &path,  
   std::string &baseName, std::string &extension )  
{  
  std::string dataFile_wrap = dataFile;  
  if (dataFile_wrap.find(".nii.gz") != std::string::npos)  
  {  
    dataFile_wrap = cbica::replaceString(dataFile_wrap, ".nii.gz", "");  
    extension = ".nii.gz";  
    std::string temp;  
    cbica::splitFileName(dataFile_wrap, path, baseName, temp);  
  }  
  else  
  {  
    //! Initialize pointers to file and user names  
f (_MSC_VER >= 1700)#i
    char basename_var[FILENAME_MAX], ext[FILENAME_MAX], path_name[FILENAME_MAX], drive_letter[FILENAME_MAX];  
    //_splitpath(dataFile_wrap.c_str(), NULL, path_name, basename_var, ext);  
    _splitpath_s(dataFile.c_str(), drive_letter, FILENAME_MAX, path_name, FILENAME_MAX, basename_var, FILENAME_MAX, ext, FILENAME_MAX);  
    path = std::string(drive_letter) + std::string(path_name);  
    path = cbica::replaceString(path, "\\", "/"); // normalize path for Windows  
lse#e
    char *basename_var, *ext, *path_name;  
    path_name = dirname(cbica::constCharToChar(dataFile_wrap.c_str()));  
    basename_var = basename(cbica::constCharToChar(dataFile_wrap.c_str()));  
    ext = strrchr(cbica::constCharToChar(dataFile_wrap.c_str()), '.');  
    path = std::string(path_name);  
ndif#e

    baseName = std::string(basename_var);  
    extension = std::string(ext);  
    
f (_MSC_VER >= 1700)#i
  path_name[0] = NULL;  
  basename_var[0] = NULL;  
  ext[0] = NULL;  
  drive_letter[0] = NULL;  
ndif#e
  if (path[path.length() - 1] != '/')  
  {  
    path += "/";  
  }  
  }  
  if (baseName == "")  
    return false;  
  else  
    return true;  
}  

/**
\brief Searches for smaller string in larger string and then replaces it with user-defined input

\param entireString String to search
\param toReplace String to replace
\param replaceWith String to replace toReplace with

\return std::string of result
*/
std::string replaceString( const std::string &entireString,   
                                  const std::string &toReplace,   
                                  const std::string &replaceWith )  
{  
  std::string return_string = entireString;  
  for( size_t pos = 0; ; pos += replaceWith.length() )   
  {  
  pos = return_string.find( toReplace, pos );  
    if( pos == std::string::npos )   
      break;  
      
    return_string.erase( pos, toReplace.length() );  
    return_string.insert( pos, replaceWith );  
  }  
  return return_string;  
}  

/**
\brief Extract all file names in supplied directory

\return Vector of file names
*/
std::vector< std::string > filesInDirectory( const std::string &dirName )  
{  
  std::vector< std::string > allFiles;  
  std::string dirName_wrap = dirName;  
  if (dirName_wrap[dirName_wrap.length()-1] != '/')  
  {  
    dirName_wrap.append("/");  
  }  
  #if defined(_WIN32)  
  {  
    dirName_wrap.append("*.*");  
    char* search_path = cbica::constCharToChar(dirName_wrap.c_str());  
    WIN32_FIND_DATA fd;   
    HANDLE hFind = ::FindFirstFile(search_path, &fd);   
    if(hFind != INVALID_HANDLE_VALUE)   
    {   
      do   
      {   
        if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )  
        {  
          allFiles.push_back(fd.cFileName);  
        }  
      } while(::FindNextFile(hFind, &fd));   
      ::FindClose(hFind);   
    }   
    return allFiles;  

  }  
  #else  
  {  
    DIR *dp;  
    struct dirent *dirp;  
    if((dp  = opendir(dirName.c_str())) == NULL)   
    {  
      std::cerr << "Error(" << errno << ") occurred while opening directory '" <<   
        dirName << "'\n";  
    }  
      
    while ((dirp = readdir(dp)) != NULL)   
    {  
      allFiles.push_back(std::string(dirp->d_name));  
    }  
    closedir(dp);  
    return allFiles;  
  }  
  #endif  
}  

// main entry of program
int main(int argc, char *argv[])
{
  try // to catch exceptions
  {
    std::string dirName = argv[1], subIDFile;
    dirName = replaceString(dirName, "\\", "/") + "/";
    std::vector< std::string > allFileNames = filesInDirectory(dirName), subjectIDs;    

    std::vector< std::tuple< std::string, std::string, std::string, std::string, std::string, std::string > > sortedFileNames;
    sortedFileNames.clear();

    bool multipleSubjects = false;

    // file name parsing
    for (size_t i = 0; i < allFileNames.size(); i++)
    {
      std::string temp_1, temp_2, ext;
      splitFileName(dirName+allFileNames[i], temp_1, temp_2, ext);
      if (ext == ".csv") // if a csv file is detected, obtain all subject IDs from it
      {
        subjectIDs.clear();
        multipleSubjects = true;
        temp_1 = dirName + "/" + allFileNames[i];
        std::ifstream infile(temp_1);
        for (std::string line; std::getline(infile, line, '\n');)
        {
          line.erase(std::remove(line.begin(), line.end(), '"'), line.end());
          subjectIDs.push_back(std::move(line)); // subject IDs are pushed onto this
        }
        goto continuation;
      }      
    }

    continuation:
    if (multipleSubjects)
    {
      for (size_t i = 0; i < subjectIDs.size(); i++)
      {
        std::string t1 = "", t2 = "", fl = "", pd = "", le = "", fg = "";
        for (size_t j = 0; j < allFileNames.size(); j++)
        {
          while ((allFileNames[j].find(subjectIDs[i]) != std::string::npos) && ((t1 == "") || (t2 == "") || (fl == "") || (pd == "") || (le == "") || (fg == "")))
          {
            if (allFileNames[j].find(subjectIDs[i]+".T1") != std::string::npos)
            {
              t1 = dirName + allFileNames[j];
            }
            else if (allFileNames[j].find(subjectIDs[i]+".T2") != std::string::npos)
            {
              t2 = dirName + allFileNames[j];
            }
            else if (allFileNames[j].find(subjectIDs[i]+".PD") != std::string::npos)
            {
              pd = dirName + allFileNames[j];
            }
            else if (allFileNames[j].find(subjectIDs[i]+".FL") != std::string::npos)
            {
              fl = dirName + allFileNames[j];
            }
            else if (allFileNames[j].find(subjectIDs[i]+".manual") != std::string::npos)
            {
              le = dirName + allFileNames[j];
            }
            else if (allFileNames[j].find(subjectIDs[i]+".foreground") != std::string::npos)
            {
              fg = dirName + allFileNames[j];
            }
            else
            {
              std::cerr << "Unsupported file name type(s).\n";
              exit(EXIT_FAILURE);
            }
            j++;
          }

          if (!((t1 == "") || (t2 == "") || (fl == "") || (pd == "") || (le == "") || (fg == "")))
          {
            sortedFileNames.push_back(std::make_tuple(t1, t2, fl, pd, fg, le)); // keeping lesions at the end because they denote labels
          }
          t1 = "", t2 = "", fl = "", pd = "", le = "", fg = "";
        }
      }
    }
    else // give use case where only a single subject ID is presented
    {
      std::string t1 = "", t2 = "", fl = "", pd = "", le = "", fg = "";
      for (size_t j = 0; j < allFileNames.size(); j++)
      {
        while ((t1 == "") || (t2 == "") || (fl == "") || (pd == "") || (le == "") || (fg == ""))
        {
          if (allFileNames[j].find("T1") != std::string::npos)
          {
            t1 = dirName + allFileNames[j];
          }
          else if (allFileNames[j].find("T2") != std::string::npos)
          {
            t2 = dirName + allFileNames[j];
          }
          else if (allFileNames[j].find("PD") != std::string::npos)
          {
            pd = dirName + allFileNames[j];
          }
          else if (allFileNames[j].find("FL") != std::string::npos)
          {
            fl = dirName + allFileNames[j];
          }
          else if (allFileNames[j].find("manual") != std::string::npos)
          {
            le = dirName + allFileNames[j];
          }
          else if (allFileNames[j].find("foreground") != std::string::npos)
          {
            fg = dirName + allFileNames[j];
          }
          else
          {
            std::cerr << "Unsupported file name type(s).\n";
          }
          j++;
        }

        if (!((t1 == "") || (t2 == "") || (fl == "") || (pd == "") || (le == "") || (fg == "")))
        {
          sortedFileNames.push_back(std::make_tuple(t1, t2, fl, pd, fg, le)); // keeping lesions at the end because they denote labels
        }
        t1 = "", t2 = "", fl = "", pd = "", le = "", fg = "";
      }
    }

    typedef float PixelType; // pre-define expected pixel type
    typedef itk::Image< PixelType, 3 > FloatImageType;

    std::vector< PixelType > t1Vector, t2Vector, pdVector, flVector, labelsVector;

    for (size_t i = 0; i < sortedFileNames.size(); i++)
    {
      FloatImageType::Pointer
        t1image = FloatImageType::New(), t2image = FloatImageType::New(), FLimage = FloatImageType::New(),
        PDimage = FloatImageType::New(), maskImage = FloatImageType::New(), lesionImage = FloatImageType::New();

      t1image     = SafeReadImage<FloatImageType>(std::get<0>(sortedFileNames[i]));
      t2image     = SafeReadImage<FloatImageType>(std::get<1>(sortedFileNames[i]));
      FLimage     = SafeReadImage<FloatImageType>(std::get<2>(sortedFileNames[i]));
      PDimage     = SafeReadImage<FloatImageType>(std::get<3>(sortedFileNames[i]));
      maskImage   = SafeReadImage<FloatImageType>(std::get<4>(sortedFileNames[i]));
      lesionImage = SafeReadImage<FloatImageType>(std::get<5>(sortedFileNames[i]));

      // initialize iterators with image and region to iterator through (in this case, it is the largest possible region)
      itk::ImageRegionIterator<FloatImageType>
        itMask(maskImage, maskImage->GetLargestPossibleRegion()),
        itT1(t1image, t1image->GetLargestPossibleRegion()),
        itT2(t2image, t2image->GetLargestPossibleRegion()),
        itFL(FLimage, FLimage->GetLargestPossibleRegion()),
        itPD(PDimage, PDimage->GetLargestPossibleRegion()),
        itLE(lesionImage, lesionImage->GetLargestPossibleRegion());

      itMask.GoToBegin(); // start mask iterator since all other computations are based on this

      std::cout << "Started reading mask '" << i << "'.\n";
      while (!itMask.IsAtEnd())
      {
        //std::cout << " " << count;
        if (itMask.Get() == static_cast<PixelType>(1))
        {
          //indexVector.push_back(itMask.GetIndex()); // obtain location of voxel in Mask

          // Set the location (i.e., Index) for other iterators
          itT1.SetIndex(itMask.GetIndex());
          itT2.SetIndex(itMask.GetIndex());
          itFL.SetIndex(itMask.GetIndex());
          itPD.SetIndex(itMask.GetIndex());
          itLE.SetIndex(itMask.GetIndex());

          // make vector of test data
          t1Vector.push_back(itT1.Get());
          t2Vector.push_back(itT1.Get());
          pdVector.push_back(itT1.Get());
          flVector.push_back(itT1.Get());
          labelsVector.push_back(itLE.Get());

          //it.Set(10);

        }
        itMask++; // only iterator through foreground mask since this all computations are based on it
      }
    }
    
    // initialize the OpenCV data structures
    cv::Mat training_data, labels(labelsVector);

    cv::hconcat(
      std::vector<cv::Mat>{cv::Mat(t1Vector), cv::Mat(t2Vector), cv::Mat(pdVector), cv::Mat(flVector)}, // make a vector of all modality intensities
      training_data); // training_data now is an [n x 4] matrix
    
    ////// start teaching the machine

    // Set up SVM's parameters. There are different parameters for different classifiers. Please see documentation for details
    cv::SVMParams params;
    params.svm_type = cv::SVM::C_SVC; // C-Support Vector Classification
    params.kernel_type = cv::SVM::LINEAR;
    params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6); // when to stop

    cv::SVM svm;
    svm.train(training_data, labels, cv::Mat(), cv::Mat(), params);

  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}