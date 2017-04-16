/* Copyright (c) <2003-2016> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/


#ifndef _D_NEWTON_H__
#define _D_NEWTON_H__


#ifdef _CNEWTON_STATIC_LIB
	#define CNEWTON_API
#else 
	#ifdef _CNEWTON_BUILD_DLL
		#ifdef _WIN32
			#define CNEWTON_API __declspec (dllexport)
		#else
			#define CNEWTON_API __attribute__ ((visibility("default")))
		#endif
	#else
		#ifdef _WIN32
			#define CNEWTON_API __declspec (dllimport)
		#else
			#define CNEWTON_API
		#endif
	#endif
#endif


#ifdef _MSC_VER
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	#endif
	#include <windows.h>
	#include <crtdbg.h>
#endif

#if ( defined (_MINGW_32_VER) || defined (_MINGW_64_VER) )
	#include <crtdbg.h>
#endif

#include <stdio.h>
#include <assert.h>
#endif

#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <Newton.h>
#include <dCustomJoint.h>
#include <dCustomGear.h>
#include <dCustomHinge.h>
#include <dCustomSlider.h>
#include <dCustomPulley.h>
#include <dCustomCorkScrew.h>
#include <dCustomUniversal.h>
#include <dCustomInputManager.h>
#include <dCustomBallAndSocket.h>
#include <dCustomHingeActuator.h>
#include <dCustomRackAndPinion.h>
#include <dCustomSliderActuator.h>
#include <dCustomTriggerManager.h>
#include <dCustomUniversalActuator.h>
#include <dCustomControllerManager.h>
#include <dCustomPlayerControllerManager.h>
#include <dCustomVehicleControllerManager.h>
#include <dCustomArcticulatedTransformManager.h>



