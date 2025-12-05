#pragma once
#include <string>




namespace Logger
{

	/// <summary>
	/// デバッグ用ログメッセージを出力ウィンドウに表示する関数。
	/// Visual Studio の「Output」欄へ文字列を送るために使用する。
	/// </summary>

	void Log(const std::string& message);


};

