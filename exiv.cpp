// ***************************************************************** -*- C++ -*-
// exifprint.cpp, $Rev: 2286 $
// Sample program to print the Exif metadata of an image

#include <exiv2/exiv2.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <glib.h>
#include <glib/gstdio.h>

#include "exiv.h"
#include "utils.h"

int xmp_read_infos(char* f,char* i1,char* i2,char* i3)
{
	Exiv2::XmpSidecar::AutoPtr sc;
	try
	{
		sc = Exiv2::ImageFactory::open(f);
	}
	catch (...)
	{
		return 0;
	}
  
  if (sc.get() == 0) return 0;
  sc->readMetadata();
  Exiv2::XmpData &xmpData = sc->xmpData();

  //we load infos
  Exiv2::XmpProperties::registerNs("http://classeimage.sf.net/", "classeimage");
  strncpy(i1,xmpData["Xmp.classeimage.info1"].toString().c_str(),1024);
  strncpy(i2,xmpData["Xmp.classeimage.info2"].toString().c_str(),1024);
  strncpy(i3,xmpData["Xmp.classeimage.info3"].toString().c_str(),1024);
 
  return 1;
}

int xmp_save_infos(char* f, char* i1, char* i2, char* i3)
{
  try
  {
    Exiv2::XmpData xmpData;
    std::string xmpPacket;
    if(g_file_test(f, G_FILE_TEST_EXISTS))
    {
      Exiv2::DataBuf buf = Exiv2::readFile(f);
      xmpPacket.assign(reinterpret_cast<char*>(buf.pData_), buf.size_);
      Exiv2::XmpParser::decode(xmpData, xmpPacket);
    }

    // initialize xmp data:
    Exiv2::XmpProperties::registerNs("http://classeimage.sf.net/", "classeimage");
    if (i1) xmpData["Xmp.classeimage.info1"] = i1;
    if (i2) xmpData["Xmp.classeimage.info2"] = i2;
    if (i3) xmpData["Xmp.classeimage.info3"] = i3;

    // serialize the xmp data and output the xmp packet
    if (Exiv2::XmpParser::encode(xmpPacket, xmpData) != 0)
    {
      throw Exiv2::Error(1, "[xmp_write] failed to serialize xmp data");
    }
    std::ofstream fout(f);
    if(fout.is_open())
    {
      fout << xmpPacket;
      fout.close();
    }
    return 0;
  }
  catch (Exiv2::AnyError& e)
  {
    std::cerr << "[xmp_write] caught exiv2 exception '" << e << "'\n";
    return -1;
  }
  return 1;
}