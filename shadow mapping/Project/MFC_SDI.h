
// MFC_SDI.h : ������� ���� ��������� ��� ���������� MFC_SDI
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"       // �������� �������


// CMFC_SDIApp:
// � ���������� ������� ������ ��. MFC_SDI.cpp
//

class CMFC_SDIApp : public CWinApp
{
public:
	CMFC_SDIApp();


// ���������������
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ����������
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFC_SDIApp theApp;
