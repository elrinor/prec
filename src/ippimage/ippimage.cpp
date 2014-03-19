/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2007 Intel Corporation. All Rights Reserved.
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

#ifndef __IPPIMAGE_H__
#include "ippimage.h"
#endif


void RGBA_FPX_to_BGRA(Ipp8u* data,int width,int height)
{
  int    i;
  int    j;
  int    pad;
  int    line_width;
  Ipp8u  r, g, b, a;
  Ipp8u* ptr;

  ptr = data;
  pad = DIB_PAD_BYTES(width,4);
  line_width = width * 4 + pad;

  for(i = 0; i < height; i++)
  {
    ptr = data + line_width*i;
    for(j = 0; j < width; j++)
    {
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      a = ptr[3];
      ptr[2] = (Ipp8u)( (r*a+1) >> 8 );
      ptr[1] = (Ipp8u)( (g*a+1) >> 8 );
      ptr[0] = (Ipp8u)( (b*a+1) >> 8 );
      ptr += 4;
    }
  }

  return;
} // RGBA_FPX_to_BGRA()


void BGRA_to_RGBA(Ipp8u* data,int width,int height)
{
  int      pad;
  int      step;
  int      order[4] = {3,2,1,0};
  Ipp8u*   ptr;
  IppiSize roi;

  ptr  = data;
  pad  = DIB_PAD_BYTES(width,4);
  step = width * 4 + pad;

  roi.height = height;
  roi.width  = width;

  ippiSwapChannels_8u_C4IR((Ipp8u*)ptr, step, roi, order);

  return;
} // BGRA_to_RGBA()


CIppImage::CIppImage(void)
{
  m_step        = 0;
  m_nchannels   = 0;
  m_precision   = 0;
  m_order       = 0;
  m_roi.width   = 0;
  m_roi.height  = 0;
  m_color       = JC_UNKNOWN;
  m_sampling    = JS_444;
  m_imageData   = 0;
  return;
} // ctor


CIppImage::~CIppImage(void)
{
  return;
} // dtor


int CIppImage::Alloc(IppiSize roi, int nchannels, int precision, int align)
{
  int r = 0;
  int size;

  m_roi.width  = roi.width;
  m_roi.height = roi.height;
  m_nchannels  = nchannels;
  m_precision  = precision;

  if(precision <= 8)
    m_step = roi.width * nchannels * sizeof(Ipp8u);
  else
    m_step = roi.width * nchannels * sizeof(Ipp16s);

  if(align)
    m_step += DIB_PAD_BYTES(roi.width,nchannels);

  size = m_step * roi.height;

  if(0 != m_imageData)
    ippFree(m_imageData);

  m_imageData = (Ipp8u*)ippMalloc(size);

  if(0 == m_imageData)
  {
    r = 1;
    goto Exit;
  }

Exit:

  return r;
} // CIppImage::Alloc()


int CIppImage::Free(void)
{
  m_roi.width  = 0;
  m_roi.height = 0;
  m_step       = 0;
  m_color      = JC_UNKNOWN;
  m_sampling   = JS_444;

  if(0 != m_imageData)
  {
    ippFree(m_imageData);
    m_imageData = 0;
  }

  return 0;
} // CIppImage::Free()


int CIppImage::ReduceBits(Ipp8u* pDst, int dstStep, IppiSize roi)
{
  Ipp16u*   p1;
  Ipp16u*   pTmpRow = 0;
  IppStatus status;

  p1 = (Ipp16u*)m_imageData;
  
  if(m_nchannels == 1)
  {
    int       nBits;
    Ipp16u    maxval = 0;

    status = ippiMax_16u_C1R(p1,m_step,roi,&maxval);
    if(ippStsNoErr != status)
      return -1;

    nBits = 0;
    while(maxval)
    {
      maxval = maxval >> 1;
      nBits++;
    }

    status = ippiLShiftC_16u_C1IR(16 - nBits,p1,m_step,roi);
    if(ippStsNoErr != status)
      return -1;

  
    status = ippiReduceBits_16u8u_C1R(p1,m_step,pDst,dstStep,roi,0,ippDitherNone,255);
    if(ippStsNoErr != status)
      return -1;
  }
  else
  {
    int       i;
    int       nBits;
    int       dstOrder[3] = {2, 1, 0};
    Ipp16u    maxval[3];
    Ipp8u*    dst;
    Ipp32u    val[3];
    IppiSize  sz;

    status = ippiMax_16u_C3R(p1, m_step, roi, maxval);
    if(ippStsNoErr != status)
      return -1;

    for(i = 0; i < 3; i++)
    {
      nBits = 0;

      while(maxval[i])
      {
        maxval[i] = maxval[i] >> 1;
        nBits++;
      }
      val[i] = 16 - nBits;   
    }

    pTmpRow = (Ipp16u*)ippMalloc(m_step);
    if(0 == pTmpRow)
      return -1;

    sz.width = roi.width;
    sz.height = 1;
    dst = pDst;
    for(i = 0; i < roi.height; i++)
    {
      status = ippiLShiftC_16u_C3R(p1,m_step,val,pTmpRow,m_step,sz);
      if(ippStsNoErr != status)
        goto Exit;

      status = ippiReduceBits_16u8u_C3R(pTmpRow,m_step,dst,dstStep,sz,0,ippDitherNone,255);
      if(ippStsNoErr != status)
        goto Exit; 

      status = ippiSwapChannels_8u_C3IR(dst, dstStep, sz, dstOrder);
      if(ippStsNoErr != status)
        goto Exit;

      p1 = (Ipp16u*)((Ipp8u*)p1 + m_step);
      dst += dstStep;
    }
  }

Exit:
  if(0 != pTmpRow)
    ippFree(pTmpRow);

  return 0;
} // CIppImage::ReduceBits()


int CIppImage::CopyBits(Ipp8u* pDst,int dstStep,IppiSize roi)
{
  IppStatus status;

  switch(m_nchannels)
  {
  case 1:
    status = ippiCopy_8u_C1R((Ipp8u*)m_imageData,m_step,pDst,dstStep,roi);
    break;

  case 3:
    status = ippiCopy_8u_C3R((Ipp8u*)m_imageData,m_step,pDst,dstStep,roi);
    break;

  case 4:
    status = ippiCopy_8u_C4R((Ipp8u*)m_imageData,m_step,pDst,dstStep,roi);
    break;

  default:
    status = ippStsErr;
    break;
  }

  return (ippStsNoErr != status);
} // CIppImage::CopyBits()


int CIppImage::CopyFrom(Ipp8u* pSrc,int srcStep,IppiSize roi)
{
  IppStatus status;

  switch(m_nchannels)
  {
  case 1:
    status = ippiCopy_8u_C1R(pSrc,srcStep,(Ipp8u*)m_imageData,m_step,roi);
    break;

  case 3:
    status = ippiCopy_8u_C3R(pSrc,srcStep,(Ipp8u*)m_imageData,m_step,roi);
    break;

  case 4:
    status = ippiCopy_8u_C4R(pSrc,srcStep,(Ipp8u*)m_imageData,m_step,roi);
    break;

  default:
    status = ippStsErr;
    break;;
  }

  return (ippStsNoErr != status);
} // CIppImage::CopyBits()


int CIppImage::CopyFrom(Ipp16s* pSrc,int srcStep,IppiSize roi)
{
  IppStatus status;

  switch(m_nchannels)
  {
  case 1:
    status = ippiCopy_16s_C1R(pSrc,srcStep,(Ipp16s*)m_imageData,m_step,roi);
    break;

  case 3:
    status = ippiCopy_16s_C3R(pSrc,srcStep,(Ipp16s*)m_imageData,m_step,roi);
    break;

  case 4:
    status = ippiCopy_16s_C4R(pSrc,srcStep,(Ipp16s*)m_imageData,m_step,roi);
    break;

  default:
    status = ippStsErr;
    break;;
  }

  return (ippStsNoErr != status);
} // CIppImage::CopyBits()


int CIppImage::SwapChannels(int * order)
{
  IppStatus status = ippStsErr;
  
  if(m_precision <= 8)
  {
    switch (m_nchannels)
    {
    case 3:
      status = ippiSwapChannels_8u_C3IR((Ipp8u*)m_imageData,m_step, m_roi, order);
      break;

    case 4:
      status = ippiSwapChannels_8u_C4IR((Ipp8u*)m_imageData,m_step, m_roi, order);
      break;
    
    default:
      break;
    }
  }

  return (ippStsNoErr != status);
} // CIppImage::SwapChannels


