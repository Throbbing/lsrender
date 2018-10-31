#pragma once



#include<cassert>
#include<cstdlib>

namespace ls
{





	class Debug
	{
	public:
		static void log(const char* format, ...);

		static void assert_msg(const char* msg);

		static void check_msg(const char* msg);


	};


#define ls_AssertMsg(e,msg) if(!(e)) \
		{	std::cerr<<msg<<"_In_"<<__FUNCTION__<<"("<<__LINE__<<")"<<std::endl;\
			std::abort();\
		}
		

#define ls_CheckMsg(e,msg) if(!(e)) std::cerr<<msg<<"_In_"<<__FUNCTION__<<"("<<__LINE__<<")"<<std::endl;


#if defined _DEBUG || DEBUG
#define ls_Assert(exp) assert(exp)
#else
#define ls_Assert(exp) (void)0
#endif

#define Unimplement std::cerr<<"Unexpected unimplemented error encountered" \
			<<"_In_"<<__FUNCTION__<<"("<<__LINE__<<")"<<std::endl;\
			std::abort();

#define ls_FCheckNZero(v) ls_CheckMsg(v == 0.f,"Invalid zero encoutered")
}