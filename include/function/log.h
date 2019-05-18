#pragma once



#include<cassert>
#include<cstdlib>
#include<stdarg.h>

namespace ls
{
#if defined _DEBUG || DEBUG
#define ls_Assert(exp) assert(exp)
#else
#define ls_Assert(exp) (void)0
#endif

#define Unimplement std::cerr<<"Unexpected unimplemented error encountered" \
			<<" in "<<"function: "<<__FUNCTION__<<" lines: "<<__LINE__<<std::endl;\
			std::abort();

#define LogVerbosity 0

	enum LogLevelFlag
	{
		EInfo = 1,
		EDebug  = 2,
		EError  = 3,
		ECrash = 4
	};



	class Debug
	{
	public:


		static void assert_msg(const char* msg);

		static void check_msg(const char* msg);


	};


	class Log
	{
	public:
		static void log(const char* format, ...);
	};




#ifndef DisableLog

#define ls_Log(format,...) ls::Log::log(format,##__VA_ARGS__ ) \
		std::cout<<" in "<<"function: "<<__FUNCTION__<<" lines: "<<__LINE__<<std::endl;

#define ls_AssertMsg(e,msg) if(!(e)) {std::cerr<<msg<<" in "<<"function: "<<__FUNCTION__<<" lines: "<<__LINE__<<std::endl;	system("pause");std::abort();}
		
		
#define ls_CheckMsg(e,msg) if(!(e)) std::cerr<<msg<<" in "<<"function: "<<__FUNCTION__<<" lines: "<<__LINE__<<std::endl;;





#define ls_FCheckNZero(v) ls_CheckMsg(v == 0.f,"Invalid zero encoutered")

#else

#define ls_Log(format,...) ;

#define ls_AssertMsg(e,msg) ;

#define ls_CheckMsg(e,msg) ;

#define ls_FCheckNZero(v) ls_CheckMsg(v == 0.f,"Invalid zero encoutered")

#endif // !DisableLog


}