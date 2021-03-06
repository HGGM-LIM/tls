

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/Segmentation_Modules/libs/)

#message(${ITK_USE_FILE} ITKUSEFILE)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE}  )

FIND_PACKAGE(VTK REQUIRED)
include(${VTK_USE_FILE})

#set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS program_options REQUIRED)
#find_package(Boost 1.59 COMPONENTS program_options REQUIRED)

INCLUDE_DIRECTORIES( ${BOOST_LIBRARYDIR} )
INCLUDE_DIRECTORIES( ${BOOST_ROOT} )
include_directories(${Boost_INCLUDE_DIRS}) 

#include_directories(${Boost_DIR})


ADD_EXECUTABLE( change_int change_intensities_main.cxx)
ADD_EXECUTABLE( copy_image copy_image.cxx) 
ADD_EXECUTABLE( image2vtkPolyData itkReadITKImageVTKPolyDataWrite.cxx )
ADD_EXECUTABLE(convert2dicom convert2dicom.cxx )
ADD_EXECUTABLE(ImageReadDicomSeriesWrite ImageReadDicomSeriesWrite.cxx )
#ADD_EXECUTABLE(vtkPolyData2obj vtkOBJWriterExample.cxx vtkOBJWriter.cxx)
ADD_EXECUTABLE(label_and_colorize label_and_colorize.cxx )
ADD_EXECUTABLE(disconnector disconnector_main.cxx )
ADD_EXECUTABLE(MedianImageFilter MedianImageFilter.cxx )
ADD_EXECUTABLE(extract_roi extract_roi_main.cxx)
ADD_EXECUTABLE(calibrate Calibrate_main.cxx)
ADD_EXECUTABLE(calculate_mean_std calculate_mean_std_main.cxx)
ADD_EXECUTABLE(read_air_calibration itkBackgroundImageFilter.txx itkBackgroundImageFilter.h
              itkMeanStdImageFilter.txx itkMeanStdImageFilter.h Adul_air.cxx)
ADD_EXECUTABLE(create_air_mask itkBackgroundImageFilter.txx itkBackgroundImageFilter.h
              itkMeanStdImageFilter.txx itkMeanStdImageFilter.h Adul_background_extractor.cxx)
ADD_EXECUTABLE(read_orientation read_orientation_main.cxx)
ADD_EXECUTABLE(apply_median apply_median.cxx )
ADD_EXECUTABLE(apply_erosion apply_erosion.cxx )
ADD_EXECUTABLE(apply_dilation apply_dilation.cxx )
ADD_EXECUTABLE(apply_gaussian apply_gaussian.cxx )
ADD_EXECUTABLE(dicom_serie2image DicomSeriesReadImageWrite2.cxx )
ADD_EXECUTABLE(TracheaTest TracheaTesting.cxx)
ADD_EXECUTABLE(TracheaTestSerie TracheaTestingSerie.cxx)
ADD_EXECUTABLE(isTrachea TracheaError.cxx)
ADD_EXECUTABLE(dicom_extractor dicom_extractor.cxx)
ADD_EXECUTABLE(simple_dicom_extractor simple_dicom_extractor.cxx)
ADD_EXECUTABLE(apply_mask apply_mask.cxx)
ADD_EXECUTABLE(hist ImageHistogram2.cxx)
ADD_EXECUTABLE(inv_image itkInvImageFilter.txx inv_image.cxx)
ADD_EXECUTABLE(find_trachea find_trachea.cxx)
ADD_EXECUTABLE(cast2char CastToChar.cxx)
ADD_EXECUTABLE(change_values ChangeValues.cxx)
ADD_EXECUTABLE(shrink Shrink.cxx)
ADD_EXECUTABLE(merge_images MergeImages.cxx)
ADD_EXECUTABLE(ResampleVolumesToBeIsotropic ResampleVolumesToBeIsotropic.cxx)

TARGET_LINK_LIBRARIES(ResampleVolumesToBeIsotropic ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(change_int ${ITK_LIBRARIES} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(copy_image ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES( image2vtkPolyData ${ITK_LIBRARIES} ${VTK_LIBRARIES})
TARGET_LINK_LIBRARIES(convert2dicom  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(ImageReadDicomSeriesWrite  ${ITK_LIBRARIES})
#TARGET_LINK_LIBRARIES(vtkPolyData2obj vtkHybrid)
#TARGET_LINK_LIBRARIES(vtkPolyData2obj ${VTK_LIBRARIES})
TARGET_LINK_LIBRARIES(label_and_colorize  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(disconnector  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(MedianImageFilter  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(extract_roi ${ITK_LIBRARIES} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(calibrate ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(calculate_mean_std ${ITK_LIBRARIES} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(read_air_calibration ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(create_air_mask ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(read_orientation  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(apply_median  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(apply_erosion  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(apply_dilation  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(apply_gaussian  ${ITK_LIBRARIES} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(dicom_serie2image  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(TracheaTest  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(TracheaTestSerie  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(isTrachea  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(dicom_extractor  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(simple_dicom_extractor  ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(apply_mask ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(hist ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(inv_image ${ITK_LIBRARIES} helpers)
TARGET_LINK_LIBRARIES(find_trachea ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(cast2char ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(change_values ${ITK_LIBRARIES})
#ADD_EXECUTABLE(create_masks create_masks.cxx)
#TARGET_LINK_LIBRARIES(create_masks ITKIO ITKCommon)
TARGET_LINK_LIBRARIES(shrink ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(merge_images ${ITK_LIBRARIES})

SET(scripts convert.sh)
FILE( COPY convert.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
#INSTALL_FILES( ${TLS_FILTERS_UNPREFIXED_BIN_INSTALL_DIR} FILES ${scripts} )


#INSTALL(TARGETS dicom_serie2image change_int copy_image image2vtkPolyData convert2dicom ImageReadDicomSeriesWrite vtkPolyData2obj label_and_colorize disconnector
#	MedianImageFilter extract_roi calibrate calculate_mean_std read_air_calibration create_air_mask read_orientation
#	apply_median apply_erosion apply_dilation apply_gaussian TracheaTest TracheaTestSerie isTrachea
#	dicom_extractor simple_dicom_extractor apply_mask hist inv_image find_trachea cast2char change_values shrink merge_images
#    RUNTIME DESTINATION ${TLS_FILTERS_BIN_INSTALL_DIR}
#)

#IF(INSTALL_DEVEL_FILES)
#    FILE(GLOB develFiles *.h *.txx *.cxx) 
#    FOREACH(f ${develFiles})
#      INSTALL_FILES(${TLS_FILTERS_DEVEL_INSTALL_DIR} FILES ${f})
#    ENDFOREACH(f)
#ENDIF(INSTALL_DEVEL_FILES)
