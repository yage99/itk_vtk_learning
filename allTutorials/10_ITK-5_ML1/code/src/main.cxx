/**
\brief 10_ITK-ML: Basic machine learning example
*/

//! ITK headers
#include "itkImage.h"
#include "vnl/vnl_matrix.h"
#include "itkImportImageFilter.h"

#include "itkOpenCVImageBridge.h"

#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"

#define ROWS 4
#define COLS 2

// main entry of program
int main(int argc, char *argv[])
{
  try // to catch exceptions
  {
    typedef float PixelType;
    typedef itk::Image< PixelType, 2 > ImageType;

    // where data starts
    ImageType::IndexType start; 
    start[0] = 0;
    start[1] = 0;

    // size of training data
    ImageType::SizeType size_training, size_labels; 
    size_training[0] = ROWS;
    size_training[1] = COLS;
    size_labels[0] = ROWS;
    size_labels[1] = 1;

    ImageType::RegionType region;
    region.SetSize(size_training);
    region.SetIndex(start);

    typedef vnl_matrix_fixed< PixelType, ROWS, COLS > MatrixType;
    typedef vnl_vector_fixed< PixelType, ROWS > VectorType;

    MatrixType training_mat;
    VectorType labels_vec;
    training_mat.set_identity();

    size_t i = 0, j = 0;

    training_mat(i, j)   = 501; // [0,0]
    training_mat(i, j+1) = 10;  // [0,1]
    labels_vec(i) = 1.0;

    i++;
    training_mat(i, j)   = 255; // [1,0]
    training_mat(i, j+1) = 10;  // [1,1]
    labels_vec(i) = -1.0;
    
    i++;
    training_mat(i, j)   = 501; // [2,0]
    training_mat(i, j+1) = 255; // [2,1]
    labels_vec(i) = -1.0;
    
    i++;
    training_mat(i, j)   = 10;  // [3,0]
    training_mat(i, j+1) = 501; // [3,1]
    labels_vec(i) = -1.0;
    
    typedef itk::ImportImageFilter< PixelType, 2 > ImportFilterType;
    ImportFilterType::Pointer import_traning = ImportFilterType::New(), import_labels = ImportFilterType::New();
    import_traning->SetRegion(region); // can be substituted by import_traning->SetOrigin() and import_traning->SetSize()
    import_traning->SetImportPointer(training_mat.data_block(), ROWS * COLS, true);
    import_traning->Update();

    // initialize and allocate memory to hold training data
    ImageType::Pointer training_itk = import_traning->GetOutput();
    // in the real world scenario, this is where you would hold your image data.
    // Take a look at http://itk.org/Doxygen/html/ImageIteratorsPage.html for details on how to iterate over an image.

    cv::Mat training_data = itk::OpenCVImageBridge::ITKImageToCVMat< ImageType >(training_itk);

    // import labels
    region.SetSize(size_labels);
    region.SetIndex(start);
    import_labels->SetRegion(region); // can be substituted by import_traning->SetOrigin() and import_traning->SetSize()
    import_labels->SetImportPointer(labels_vec.data_block(), ROWS * 1, true);
    import_labels->Update();

    cv::Mat labels_data = itk::OpenCVImageBridge::ITKImageToCVMat< ImageType >(import_labels->GetOutput());

    // fix orientation. This is required because of the difference in the way OpenCV and ITK handle axes
    training_data = training_data.reshape(1, ROWS);
    cv::transpose(labels_data, labels_data); // cane be substituted by labels_data = labels_data.reshape(1, 4);

    // Set up SVM's parameters. There are different parameters for different classifiers. Please see documentation for details
    cv::SVMParams params;
    params.svm_type = CvSVM::C_SVC; // C-Support Vector Classification
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6); // when to stop

    cv::SVM svm;
    svm.train(training_data, labels_data, cv::Mat(), cv::Mat(), params);

    int width = 512, height = 512;
    cv::Mat image = cv::Mat::zeros(height, width, CV_8UC3); // this image is constructed purely for visualization purposes

    cv::Vec3b green(0, 255, 0), blue(255, 0, 0); // OpenCV's color space is BGR while ITK's is RGB
    // Show the decision regions given by the SVM
    for (int i = 0; i < image.rows; ++i)
      for (int j = 0; j < image.cols; ++j)
      {
        cv::Mat sampleMat = (cv::Mat_<float>(1, 2) << j, i); // construct a sample from the rows and cols indices
        float response = svm.predict(sampleMat);

        if (response == 1)
          image.at<cv::Vec3b>(i, j) = green; 
        else if (response == -1)
          image.at<cv::Vec3b>(i, j) = blue;
      }

    // Show the training data
    int thickness = -1;
    int lineType = 8;
    cv::circle(image, cv::Point(501, 10), 5, cv::Scalar(0, 0, 0), thickness, lineType);
    cv::circle(image, cv::Point(255, 10), 5, cv::Scalar(255, 255, 255), thickness, lineType);
    cv::circle(image, cv::Point(501, 255), 5, cv::Scalar(255, 255, 255), thickness, lineType);
    cv::circle(image, cv::Point(10, 501), 5, cv::Scalar(255, 255, 255), thickness, lineType);

    // Show support vectors
    thickness = 2;
    lineType = 8;
    int c = svm.get_support_vector_count();

    for (int i = 0; i < c; ++i)
    {
      const float* v = svm.get_support_vector(i);
      cv::circle(image, cv::Point((int)v[0], (int)v[1]), 6, cv::Scalar(128, 128, 128), thickness, lineType);
    }

    cv::imwrite("result.png", image);        // save the image

    cv::imshow("SVM Simple Example", image); // show it to the user
    cv::waitKey(0);

  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}