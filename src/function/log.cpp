#include<function/log.h>
#include<iostream>
#include<cstdlib>
#include<csignal>
void ls::Debug::log(const char * format, ...)
{
	Unimplement;
}

void ls::Debug::assert_msg(const char * msg)
{
	std::cerr << msg << "_In_" << __FUNCTION__ << "("<<__LINE__<<")"<< std::endl;
	std::abort();
	
}

void ls::Debug::check_msg(const char * msg)
{
	std::cerr << msg << "_In_" << __FUNCTION__ << "(" << __LINE__ << ")" << std::endl;
	std::system("pause");
}
