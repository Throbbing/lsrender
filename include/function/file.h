#pragma once

#include<config\config.h>

/*
path.h -- A simple class for manipulating paths on Linux/Windows/Mac OS

Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>

All rights reserved. Use of this source code is governed by a
BSD-style license that can be found in the LICENSE file.
*/



#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#if defined(_WIN32)
# include <windows.h>
#else
# include <unistd.h>
#endif
#include <sys/stat.h>

#if defined(__linux)
# include <linux/limits.h>
#endif

namespace ls
{

	/**
	* \brief Simple class for manipulating paths on Linux/Windows/Mac OS
	*
	* This class is just a temporary workaround to avoid the heavy boost
	* dependency until boost::filesystem is integrated into the standard template
	* library at some point in the future.
	*/

	std::wstring StringToWstring(const std::string str)
	{
		unsigned len = str.size() * 2;// 预留字节数
		setlocale(LC_CTYPE, "");     //必须调用此函数
		wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
		mbstowcs(p,str.c_str(), len);// 转换
		std::wstring str1(p);
		delete[] p;// 释放申请的内存
		return str1;
	}

	std::string WstringToString(const std::wstring str)
	{
		unsigned len = str.size() * 4;
		setlocale(LC_CTYPE, "");
		char *p = new char[len];
		wcstombs(p, str.c_str(), len);
		std::string str1(p);
		delete[] p;
		return str1;
	}

	class Path {
	public:
		static std::wstring StringToWstring(const std::string str)
		{
			unsigned len = str.size() * 2;// 预留字节数
			setlocale(LC_CTYPE, "");     //必须调用此函数
			wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
			mbstowcs(p, str.c_str(), len);// 转换
			std::wstring str1(p);
			delete[] p;// 释放申请的内存
			return str1;
		}

		static std::string WstringToString(const std::wstring str)
		{
			unsigned len = str.size() * 4;
			setlocale(LC_CTYPE, "");
			char *p = new char[len];
			wcstombs(p, str.c_str(), len);
			std::string str1(p);
			delete[] p;
			return str1;
		}


		enum path_type {
			windows_path = 0,
			posix_path = 1,
#if defined(_WIN32)
			native_path = windows_path
#else
			native_path = posix_path
#endif
		};

		Path() : m_type(native_path), m_absolute(false) { }

		Path(const Path &path)
			: m_type(path.m_type), m_path(path.m_path), m_absolute(path.m_absolute) {}

		Path(Path &&path)
			: m_type(path.m_type), m_path(std::move(path.m_path)),
			m_absolute(path.m_absolute) {}

		Path(const char *string) { set(string); }

		Path(const std::string &string) { set(string); }

#if defined(_WIN32)
		Path(const std::wstring &wstring) { set(wstring); }
		Path(const wchar_t *wstring) { set(wstring); }
#endif

		size_t length() const { return m_path.size(); }

		bool empty() const { return m_path.empty(); }

		bool isAbsolute() const { return m_absolute; }

		Path makeAbsolute() const {

		std::wstring value = wstr(), out(MAX_PATH, '\0');
		DWORD length = GetFullPathNameW(value.c_str(), MAX_PATH, &out[0], NULL);
		if (length == 0)
			throw std::runtime_error("Internal error in realpath(): " + std::to_string(GetLastError()));
		return Path(out.substr(0, length));

		}

		bool exists() const {
			return GetFileAttributesW(wstr().c_str()) != INVALID_FILE_ATTRIBUTES;

		}

		size_t fileSize() const {
			struct _stati64 sb;
			if (_wstati64(wstr().c_str(), &sb) != 0)
				throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");

			return (size_t)sb.st_size;
		}

		bool isDirectory() const {
			DWORD result = GetFileAttributesW(wstr().c_str());
			if (result == INVALID_FILE_ATTRIBUTES)
				return false;
			return (result & FILE_ATTRIBUTE_DIRECTORY) != 0;
		}

		bool isFile() const {
			DWORD attr = GetFileAttributesW(wstr().c_str());
			return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
		}

		std::string extension() const {
			const std::string &name = filename();
			size_t pos = name.find_last_of(".");
			if (pos == std::string::npos)
				return "";
			return name.substr(pos + 1);
		}

		std::string filename() const {
			if (empty())
				return "";
			const std::string &last = m_path[m_path.size() - 1];
			return last;
		}

		Path parentPath() const {
			Path result;
			result.m_absolute = m_absolute;

			if (m_path.empty()) {
				if (!m_absolute)
					result.m_path.push_back("..");
			}
			else {
				size_t until = m_path.size() - 1;
				for (size_t i = 0; i < until; ++i)
					result.m_path.push_back(m_path[i]);
			}
			return result;
		}

		Path operator/(const Path &other) const {
			if (other.m_absolute)
				throw std::runtime_error("path::operator/(): expected a relative path!");
			if (m_type != other.m_type)
				throw std::runtime_error("path::operator/(): expected a path of the same type!");

			Path result(*this);

			for (size_t i = 0; i < other.m_path.size(); ++i)
				result.m_path.push_back(other.m_path[i]);

			return result;
		}

		std::string str(path_type type = native_path) const {
			std::ostringstream oss;

			if (m_type == posix_path && m_absolute)
				oss << "/";

			for (size_t i = 0; i < m_path.size(); ++i) {
				oss << m_path[i];
				if (i + 1 < m_path.size()) {
					if (type == posix_path)
						oss << '/';
					else
						oss << '\\';
				}
			}

			return oss.str();
		}


		void set(const std::string &str, path_type type = native_path) {
			m_type = type;
			if (type == windows_path) {
				m_path = tokenize(str, "/\\");
				m_absolute = str.size() >= 2 && std::isalpha(str[0]) && str[1] == ':';
			}
			else {
				m_path = tokenize(str, "/");
				m_absolute = !str.empty() && str[0] == '/';
			}
		}

		Path &operator=(const Path &path) {
			m_type = path.m_type;
			m_path = path.m_path;
			m_absolute = path.m_absolute;
			return *this;
		}

		Path &operator=(Path &&path) {
			if (this != &path) {
				m_type = path.m_type;
				m_path = std::move(path.m_path);
				m_absolute = path.m_absolute;
			}
			return *this;
		}

		friend std::ostream &operator<<(std::ostream &os, const Path &path) {
			os << path.str();
			return os;
		}

		bool removeFile()
		{
			return DeleteFileW(wstr().c_str()) != 0;
		}

		bool resizeFile(size_t target_length) {

			HANDLE handle = CreateFileW(wstr().c_str(), GENERIC_WRITE, 0, nullptr, 0, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (handle == INVALID_HANDLE_VALUE)
				return false;
			LARGE_INTEGER size;
			size.QuadPart = (LONGLONG)target_length;
			if (SetFilePointerEx(handle, size, NULL, FILE_BEGIN) == 0) {
				CloseHandle(handle);
				return false;
			}
			if (SetEndOfFile(handle) == 0) {
				CloseHandle(handle);
				return false;
			}
			CloseHandle(handle);
			return true;
		}

		static Path getcwd() {
			std::wstring temp(MAX_PATH, '\0');
			if (!_wgetcwd(&temp[0], MAX_PATH))
				throw std::runtime_error("Internal error in getcwd(): " + std::to_string(GetLastError()));
			return Path(temp.c_str());
		}

#if defined(_WIN32)
		std::wstring wstr(path_type type = native_path) const {
			std::string temp = str(type);
			int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
			std::wstring result(size, 0);
			MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &result[0], size);
			return result;
		}


		void set(const std::wstring &wstring, path_type type = native_path) {
			std::string string;
			if (!wstring.empty()) {
				int size = WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(),
					NULL, 0, NULL, NULL);
				string.resize(size, 0);
				WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(),
					&string[0], size, NULL, NULL);
			}
			set(string, type);
		}

		Path &operator=(const std::wstring &str) { set(str); return *this; }
#endif

		bool operator==(const Path &p) const { return p.m_path == m_path; }
		bool operator!=(const Path &p) const { return p.m_path != m_path; }

	protected:
		static std::vector<std::string> tokenize(const std::string &string, const std::string &delim) {
			std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
			std::vector<std::string> tokens;

			while (lastPos != std::string::npos) {
				if (pos != lastPos)
					tokens.push_back(string.substr(lastPos, pos - lastPos));
				lastPos = pos;
				if (lastPos == std::string::npos || lastPos + 1 == string.length())
					break;
				pos = string.find_first_of(delim, ++lastPos);
			}

			return tokens;
		}

	protected:
		path_type m_type;
		std::vector<std::string> m_path;
		bool m_absolute;
	};

	inline bool create_directory(const Path& p) {
#if defined(_WIN32)
		return CreateDirectoryW(p.wstr().c_str(), NULL) != 0;
#else
		return mkdir(p.str().c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0;
#endif
	}

}

