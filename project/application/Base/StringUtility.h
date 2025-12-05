#pragma once
#include <string>
namespace StringUtility
{
	/// <summary>
	/// UTF-8 の std::string を UTF-16 の std::wstring に変換する。
	/// </summary>
	std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// UTF-16 の std::wstring を UTF-8 の std::string に変換する。
	/// </summary>
	std::string ConvertString(const std::wstring& str);



};

