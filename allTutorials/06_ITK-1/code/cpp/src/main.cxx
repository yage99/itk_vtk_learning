/**
\brief 09_ITK-1: Read/Write Image and matrix operations
*/

//! ITK headers
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImportImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkCastImageFilter.h"

// matrix headers
#include "itkMatrix.h"
#include "vnl/vnl_math.h"
#include "vnl/vnl_matrix.h"
 
/**
\brief Get the itk::Image

\param The itk::Image which will contain the image data
*/
template <typename TImageType>
/*TImageType::Pointer*/void ReadImage(typename TImageType::Pointer image, std::string fName)
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
  return /*image*/;
}

/**
\brief Apply matrix multiplication
*/
template <typename TImageType>
void matrixManipulation( typename TImageType::Pointer image, 
                         typename TImageType::Pointer mask,
                         const std::string &fOutName )
{
  typedef double OPixelType;
  typedef itk::Image< OPixelType, 2 > OImageType;
  typedef itk::CastImageFilter< TImageType, OImageType > CastFilterType;
  typename CastFilterType::Pointer filter = CastFilterType::New();

  filter->SetInput(image);
  OImageType::Pointer image_2 = filter->GetOutput();

  filter->SetInput(mask);
  OImageType::Pointer mask_2 = filter->GetOutput();

  typedef vnl_matrix< OPixelType > MatrixType;
  
  const unsigned int rows = image_2->GetBufferedRegion().GetSize()[0];
  const unsigned int cols = image_2->GetBufferedRegion().GetSize()[1];

  MatrixType matrix_1(image_2->GetBufferPointer(), rows, cols);
  MatrixType matrix_2(mask_2->GetBufferPointer(), rows, cols);

  MatrixType multiply = matrix_1 * matrix_2;
  //MatrixType inverse = vnl_matrix_inverse<OPixelType>(matrix_1);

  typedef itk::ImportImageFilter< OPixelType, 2 > ImportFilterType;
  ImportFilterType::Pointer result_multiply = ImportFilterType::New();
  result_multiply->SetSpacing(image_2->GetSpacing());
  result_multiply->SetOrigin(image_2->GetOrigin());
  result_multiply->SetImportPointer(multiply.data_block(), rows*cols, true);

  typedef itk::ImageFileWriter< OImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(fOutName);
  writer->SetInput(result_multiply->GetOutput());
  writer->Update(); // Write() can also be used; U

}

/**
\brief Apply multiplication filter
*/
template <typename TImageType>
void filterMultiplcation( typename TImageType::Pointer image,
                          typename TImageType::Pointer mask,
                          const std::string &fOutName)
{
  typedef itk::MultiplyImageFilter<TImageType, TImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput1(image);
  filter->SetInput2(mask);
  filter->Update();
  typename TImageType::Pointer result = filter->GetOutput();

  typedef itk::ImageFileWriter<TImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(fOutName);
  writer->Write();
}

// main entry of program
int main(int argc, char *argv[])
{
  try // to catch exceptions
  {
    printf("hello\n");
    // basic check to see image file has been put in by the user
    if( (argc < 3) || (argc > 5) )
    {
      std::cerr << "Usage: " << std::endl;
      std::cerr << argv[0] << " <inputImageFile> <maskImageFile> <outputImageFile> <f> for filter multiply\n" << std::endl;
      return EXIT_FAILURE;
    }

    bool matrixFlag = true;

    if (argv[4] == "f")
    {
      matrixFlag = false;
    }

    itk::ImageIOBase::Pointer im_base = itk::ImageIOFactory::CreateImageIO(argv[1], itk::ImageIOFactory::ReadMode);

    im_base->SetFileName(argv[1]);
    im_base->ReadImageInformation();

    const auto ComponentType = im_base->GetComponentType();
    const unsigned int dimensions = im_base->GetNumberOfDimensions();
    printf("dimensions: %d\n", dimensions);

    switch (im_base->GetPixelType())
    {
    case itk::ImageIOBase::SCALAR:
    {
      switch (dimensions)
      {
      case 2:

      {
        switch (im_base->GetComponentType())
        {
        case itk::ImageIOBase::UCHAR:
        {
          typedef unsigned char PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());
          
          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        case itk::ImageIOBase::CHAR:
        {
          typedef char PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());

          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        case itk::ImageIOBase::USHORT:
        {
          typedef unsigned short PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());

          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        case itk::ImageIOBase::SHORT:
        {
          typedef short PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());

          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        case itk::ImageIOBase::UINT:
        {
          typedef unsigned int PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());

          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        case itk::ImageIOBase::INT:
        {
          typedef int PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());

          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        case itk::ImageIOBase::ULONG:
        {
          typedef unsigned long PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());

          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        case itk::ImageIOBase::LONG:
        {
          typedef long PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());

          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        case itk::ImageIOBase::FLOAT:
        {
          typedef float PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());

          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        case itk::ImageIOBase::DOUBLE:
        {
          typedef double PixelType;
          typedef itk::Image<PixelType, 2> ImageType;
          ImageType::Pointer image = ImageType::New();
          ReadImage<ImageType>(image, im_base->GetFileName());

          ImageType::Pointer mask = ImageType::New();
          ReadImage<ImageType>(mask, argv[2]);

          if (matrixFlag)
          {
            matrixManipulation<ImageType>(image, mask, argv[3]);
          }
          else
          {
            filterMultiplcation<ImageType>(image, mask, argv[3]);
          }

        }
        break;

        default:
        {
          std::cerr << "Unsupported component type. Supported Types are:\n" <<
            "signed/unsigned short, signed/unsigned int, signed/unsigned long, float, double\n";
          break;
        }
        }
      }
      break;
      
      default:
      {
        std::cerr << "Unsupported dimension size. Supported dimension types:\n" <<
          "2, 3\n";
        break;
      }
      }
    }
    break;

    default:
    {
      std::cerr << "Unsupported image type. Supported Image types:\n" <<
        "Scalar\n";
      break;
    }
    }

  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
