
// TicTacToe.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CTicTacToeApp:
// Сведения о реализации этого класса: TicTacToe.cpp
//

class CTicTacToeApp : public CWinApp
{
public:
	CTicTacToeApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CTicTacToeApp theApp;
