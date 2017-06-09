#! /usr/bin/env python

import sys
sys.path.append('/sbia/sbiasfw/external/ITK/centos6/4.7.0/lib/ITK-4.7/Python/')
import itk

def main(argv=None):
    if argv is None:
        argv=sys.argv
        argc=len(sys.argv)
    if argc != 4: #name of script and 3 args
        sys.stderr.write("Usage: \n")
        sys.stderr.write(argv[0]+" <inputImageFileName> <image2ImageFileName> <outputImageFileName> <f> for filter multiply\n")
        exit(2)
        
    image1Filename=argv[1]
    image2Filename=argv[2]
    outputFilename=argv[3]
    
    #Read Images
    image1Reader,image1Type=readImageWithItk(image1Filename)
    image2Reader,image2Type=readImageWithItk(image2Filename)
    
    matrixFlag=True
    
    if argv[4]:
        matrixFlag=False
    
    #Write the output to file
    writerType=itk.ImageFileWriter[image1Type]
    writer=writerType.New()
    writer.SetFileName(outputFilename)
        
    if matrixFlag:
        writer.SetInput(matrixManipulation(image1Type,image1Reader,image2Reader))
    else:
        writer.SetInput(filterMultiplication(image1Type,image2Type,image1Reader,image2Reader))
    
    writer.SetInput(filter.GetOutput())
    
    #hope for the best
    writer.Update()
    
    exit(0)
    
def filterMultiplication(image1Type,image2Type,image1Reader,image2Reader):
    filterType=itk.MultiplyImageFilter[image1Type,image2Type,image1Type]
    filter=filterType.New()
    filter.SetInput1(image1Reader.GetOutput())
    filter.SetInput2(image2Reader.GetOutput())
    filter.Update()
    
    return filter.GetOutput()
    
def matrixManipulation(IImageType,image1Reader,image2Reader)
    OImageType=itk.Image[itk.D,2]
    CastFilterType=itk.CastImageFilter[IImageType,OImageType]
    filter=CastFilterType.New()
    
    filter.SetInput(image1Reader.GetOutput())
    image_2=filter.GetOutput()

    filter.SetInput(image2Reader.GetOutput())
    mask_2=filter.GetOutput()
    
    rows=image_2.GetBufferedRegion().GetSize()[0]
    cols=image_2.GetBufferedRegion().GetSize()[1]
    
    m_1=itk.vnl_matrix[itk.D]
    m_2=itk.vnl_matrix[itk.D]    
    m_1(image_2.GetBufferPointer(), rows, cols);
    m_2(mask_2.GetBufferPointer(), rows, cols);
    
    multiply=itk.vnl_matrix[itk.D]
    multiply=m_1 * m_2
    
    #inverse=itk.vnl_matrix[itk.D]
    #inverse=itk.vnl_matrix_inverse[itk.D](m_1)
    
    importFilterType=itk.ImportImageFilter[itk.D,2]
    result_multiply=importFilterType.New()
    result_multiply.SetSpacing(image_2.GetSpacing());
    result_multiply.SetOrigin(image_2.GetOrigin());
    result_multiply.SetImportPointer(multiply.data_block(), rows*cols, True);
  
    return result_multiply.GetOutput()
    
    
def getItkComponentType(ImageIOInstance):
    componentType=ImageIOInstance.GetComponentType()
    return {
        0: None,
        1: itk.UC,
        2: itk.SC,
        3: itk.US,
        4: itk.SS,
        5: itk.UI,
        6: itk.SI,
        7: itk.UL,
        8: itk.SL,
        9: itk.F,
        10: itk.D,
        }[componentType]
    
def readImageWithItk(imageFilename):
    imageInput=itk.ImageIOFactory.CreateImageIO(imageFilename,itk.ImageIOFactory.ReadMode)
    imageInput.SetFileName(imageFilename)
    imageInput.ReadImageInformation()
    imageComponentType=getItkComponentType(imageInput)
    imageNumDims=imageInput.GetNumberOfDimensions()
    imageType=itk.Image[imageComponentType,imageNumDims]
    imageReaderType=itk.ImageFileReader[imageType]
    imageReader=imageReaderType.New()
    imageReader.SetFileName(imageFilename)
    imageReader.Update()
    return imageReader,imageType
    
if __name__ == '__main__': main() 