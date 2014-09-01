Implementation of "Digital matting and compositing" thesis work.
Author: Savvas Christodoulou
Contact: schris@cs.ucy.ac.cy
Cyprus, June 2014

Contents
--------

NearestSampleMatting.h
Nearest sample matting algorithm header file

NearestSampleMatting.cpp 
Nearest sample matting algorithm source file

NearestSampleMattingApp.cpp
Nearest sample matting demo


Prerequisites
-------------

Windows 7,8
Visual Studio 2012
OpenCV 2.4.8
Visual C++ Redistributable 2012

Set-Up Guide
------------
Modify existing property sheets with your own opencv installation directories or delete existing ones and create new property sheets for opencv reference.

For development with pre-compiled DLL:
In the property pages of your project reference NearestSampleMatting.dll and NearestSampleMatting.h.

For development with library source:
Create your project in the same solution as the library and in the property pages of your project reference NearestSampleMatting.dll and NearestSampleMatting.h.

For more info http://msdn.microsoft.com/en-us/library/ms235636.aspx last accessed June 2014.
