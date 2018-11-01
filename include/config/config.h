
#pragma once

#include<iostream>
#include<cfloat>
#include<cassert>
#include<iostream>
#include<cstdlib>
#include<vector>
#include<sstream>
#include<cstdio>
#include<string>
#include<memory>
#include<fstream>



#define ls_Embree3

#if defined ls_Embree2
#include<embree2/rtcore.h>
#include<embree2/rtcore_ray.h>
#elif defined ls_Embree3
#include<embree3/rtcore.h>
#include<embree3/rtcore_ray.h>
#endif



#define ls_Param_Out
#define ls_Param_In

#ifndef ls_ForceInline

#define ls_ForceInline __forceinline

#endif 

#ifndef ls_Isnan
#define ls_Isnan  isnan
#endif

#ifndef ls_Smart
#define ls_Smart(argType,var)  std::shared_ptr<argType> var
#endif

#ifndef ls_MakeSmart
#define ls_MakeSmart(argType) std::make_shared<argType>
#endif // !ls_MakeSmart


#ifndef ls_Unique
#define ls_Unique(argType,var) std::unique_ptr<argType> var
#endif

#ifndef ls_Export
#define ls_Export export
#endif


#ifndef ls_Import
#define ls_Import import
#endif

#ifndef ls_Unused
#define ls_Unused [[maybe_unused]]
#endif // ls_DebugVar


#ifndef ls_Align

#define ls_Align(n) __declspec(align(n))

#endif 


using s8 = __int8;
using s16 = __int16;
using s32 = __int32;
using s64 = __int64;
using u8 = unsigned __int8;
using u16 = unsigned __int16;
using u32 = unsigned __int32;
using u64 = unsigned __int64;
using f32 = float;
using f64 = double;


#pragma warning (disable : 4305) // double constant assigned to float
#pragma warning (disable : 4244) // int -> float conversion
#pragma warning (disable : 4267) // size_t -> unsigned int conversion

namespace ls
{
	
	




	
}