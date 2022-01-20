
// MFC_SDI.h : главный файл заголовка для приложения MFC_SDI
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CMFC_SDIApp:
// О реализации данного класса см. MFC_SDI.cpp
//

class CMFC_SDIApp : public CWinApp
{
public:
	CMFC_SDIApp();


// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFC_SDIApp theApp;
