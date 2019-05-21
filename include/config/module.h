#pragma once
#include<config/config.h>


namespace ls
{
	/*
		module class 
		which is treated as basic interface in lsrender
	*/
	class Module
	{
	public:
		Module(const std::string& _ID):ID(_ID) {}
		virtual ~Module() {}

		

		/*
			commit any changes into context
			must call commit function before using module, otherwise leads to undefine behavior
		*/
		virtual void commit() = 0;


		/*
			print this module as a string
			for debug
		*/
		virtual std::string strOut() const = 0;
		

		/*
			wake this module
			for hierarchy scene
		*/
		virtual void wake() {}
		virtual bool isWake() { return true; }

		/*
			suspend this module
			for hierarchy scene
		*/
		virtual void suspend() {}
		virtual bool isSuspend() { return false; }

	protected:
		std::string						ID;
	};
}