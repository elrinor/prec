/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2006 Intel Corporation. All Rights Reserved.
//
//  Intel® Integrated Performance Primitives JPEG Viewer Sample for Windows*
//
//  By downloading and installing this sample, you hereby agree that the
//  accompanying Materials are being provided to you under the terms and
//  conditions of the End User License Agreement for the Intel® Integrated
//  Performance Primitives product previously accepted by you. Please refer
//  to the file ippEULA.rtf or ippEULA.txt located in the root directory of your Intel® IPP product
//  installation for more information.
//
//  JPEG is an international standard promoted by ISO/IEC and other organizations.
//  Implementations of these standards, or the standard enabled platforms may
//  require licenses from various entities, including Intel Corporation.
//
//
*/

#include <stdlib.h>
//#ifdef WIN32
//#define WIN32_LEAN_AND_MEAN
//#include "windows.h"
//#endif
#ifndef __BMP_H__
#include "bmp.h"
#endif
#ifndef __IPPIMAGE_H__
#define __IPPIMAGE_H__
#endif

JERRCODE ReadImageBMP(
  CBaseStreamInput* in,
  CIppImage*        image)
{
  int      r;
  int      i;
  int      nchannels;
  Ipp8u*   ptr;
  IppiSize roi;
  uic_size_t cnt;
  JERRCODE jerr;
  ImageFileHeader bmfh;
  ImageHeader bmih;
  RGBquad palette[256];

  jerr = in->Read(&bmfh,sizeof(ImageFileHeader),&cnt);
  if(JPEG_OK != jerr)
    return jerr;

  if(bmfh.bfType != 'MB')
  {
    return JPEG_ERR_BAD_DATA;
  }

  jerr = in->Read(&bmih,sizeof(ImageHeader),&cnt);
  if(JPEG_OK != jerr)
    return jerr;

  if(bmih.biSize != sizeof(ImageHeader))
  {
    return JPEG_ERR_BAD_DATA;
  }

  if(bmih.biCompression != _BI_RGB || (bmih.biBitCount != 8 && bmih.biBitCount != 24))
  {
    return JPEG_NOT_IMPLEMENTED;
  }

  if(bmih.biBitCount == 8)
  {
    jerr = in->Read(palette,sizeof(RGBquad)*256,&cnt);
    if(JPEG_OK != jerr)
      return jerr;
  }

  roi.width  = bmih.biWidth;
  roi.height = abs(bmih.biHeight);

  nchannels = bmih.biBitCount >> 3;

  switch(nchannels)
  {
  case 1:  image->Color(JC_GRAY);    break;
  case 3:  image->Color(JC_BGR);     break;
  case 4:  image->Color(JC_BGRA);    break;
  default: image->Color(JC_UNKNOWN); break;
  }

  r = image->Alloc(roi,nchannels,8,1);
  if(0 != r)
  {
    return JPEG_ERR_ALLOC;
  }

  jerr = in->Seek(bmfh.bfOffBits,UIC_SEEK_SET);
  if(JPEG_OK != jerr)
    return jerr;

  // read bottom-up BMP
  ptr = (Ipp8u*)*image + image->Step() * (roi.height - 1);

  for(i = 0; i < roi.height; i++)
  {
    jerr = in->Read(ptr - i * image->Step(),image->Step(),&cnt);
    if(JPEG_OK != jerr)
      return jerr;
  }

  return JPEG_OK;
} // ReadImageBMP()


JERRCODE SaveImageBMP(
  CIppImage*         image,
  CBaseStreamOutput* out)
{
  int              i;
  int              pad;
  int              imageStep;
  int              imageSize;
  int              fileSize;
  uic_size_t       cnt;
  Ipp8u*           ptr;
  ImageFileHeader  bmfh;
  ImageHeader      bmih;
  RGBquad          palette[256];
  JERRCODE         jerr;

  pad       = DIB_PAD_BYTES(image->Width(),image->NChannels());
  imageStep = image->Width() * image->NChannels() + pad;
  imageSize = image->Step() * image->Height();

  fileSize  = imageSize + sizeof(ImageFileHeader) + sizeof(ImageHeader);

  bmfh.bfType      = 'MB';
  bmfh.bfSize      = fileSize;
  bmfh.bfReserved1 = 0;
  bmfh.bfReserved2 = 0;
  bmfh.bfOffBits   = sizeof(ImageFileHeader) + sizeof(ImageHeader);

  if(image->NChannels() == 1)
  {
    bmfh.bfOffBits += sizeof(palette);
  }

  jerr = out->Write(&bmfh,sizeof(ImageFileHeader),&cnt);
  if(JPEG_OK != jerr)
    return jerr;

  bmih.biSize          = sizeof(ImageHeader);
  bmih.biWidth         = image->Width();
  bmih.biHeight        = image->Height();
  bmih.biPlanes        = 1;
  bmih.biBitCount      = (unsigned short)(image->NChannels() << 3);
  bmih.biCompression   = _BI_RGB;  
  bmih.biSizeImage     = imageSize;
  bmih.biXPelsPerMeter = 0;
  bmih.biYPelsPerMeter = 0;
  bmih.biClrUsed       = image->NChannels() == 1 ? 256 : 0;
  bmih.biClrImportant  = image->NChannels() == 1 ? 256 : 0;

  jerr = out->Write(&bmih,sizeof(ImageHeader),&cnt);
  if(JPEG_OK != jerr)
    return jerr;

  if(image->NChannels() == 1)
  {
    for(i = 0; i < 256; i++)
    {
      palette[i].rgbBlue     = (Ipp8u)i;
      palette[i].rgbGreen    = (Ipp8u)i;
      palette[i].rgbRed      = (Ipp8u)i;
      palette[i].rgbReserved = (Ipp8u)0;
    }

    jerr = out->Write(&palette,sizeof(palette),&cnt);
    if(JPEG_OK != jerr)
      return jerr;
  }

  // write bottom-up BMP
  ptr = (Ipp8u*)*image + image->Step() * (image->Height() - 1);

  for(i = 0; i < image->Height(); i++)
  {
    jerr = out->Write(ptr - i * image->Step(),image->Step(),&cnt);
    if(JPEG_OK != jerr)
      return jerr;
  }

  return JPEG_OK;
} // SaveImageBMP()

