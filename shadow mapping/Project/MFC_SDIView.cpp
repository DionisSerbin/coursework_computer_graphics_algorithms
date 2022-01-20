// MFC_SDIView.cpp : ���������� ������ CMFC_SDIView
//

#include "stdafx.h"
// SHARED_HANDLERS ����� ���������� � ������������ �������� ��������� ���������� ������� ATL, �������
// � ������; ��������� ��������� ������������ ��� ��������� � ������ �������.
#ifndef SHARED_HANDLERS
#include "MFC_SDI.h"
#endif

#include "MFC_SDIDoc.h"
#include "MFC_SDIView.h"

//#include "Komnata.h"
#include "Kamera.h"

#include <windows.h>									// Header File For Windows
#include <gl.h>										// Header File For The OpenGL32 Library
#include <glu.h>										// Header File For The GLu32 Library
#include <glaux.h>									// Header File For The Glaux Library

#include "math.h"									// Header File For 3D Object Handling


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC_SDIView

IMPLEMENT_DYNCREATE(CMFC_SDIView, CView)

BEGIN_MESSAGE_MAP(CMFC_SDIView, CView)
	// ����������� ������� ������
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_BUTTON32771, &CMFC_SDIView::OnButton32771)
//	ON_WM_KEYDOWN()
//	ON_WM_KEYUP()
ON_WM_CHAR()
ON_WM_KEYDOWN()
ON_WM_LBUTTONDOWN()
ON_COMMAND(ID_BUTTON32777, &CMFC_SDIView::OnButton32777)
END_MESSAGE_MAP()

//�������� ����������
GLboolean ambientShadowAvailable = GL_FALSE;
GLboolean controlCamera = GL_TRUE;      // ������� xyz ����� ��������� lightpos
GLboolean noShadows = GL_FALSE;         // ���������� ���������
GLboolean showShadowMap = GL_FALSE;     // �������� �������� ������� �����

GLfloat factor = 4.0f;                  // ��� �������� ���������

GLint windowWidth = 512;                // ������ ����
GLint windowHeight = 512;

GLint shadowSize = 512;                 // ��������������� � ����������� �� ������� ����
GLuint shadowTextureID;

GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat noLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightPos[] = { 100.0f, 300.0f, 100.0f, 1.0f };
GLfloat cameraPos[] = { 100.0f, 150.0f, 200.0f, 1.0f };



//����������� ������
CMFC_SDIView::CMFC_SDIView()
{
	// TODO: �������� ��� ��������
	alpha_ = 0;
	beta_ = 0;
	scale_ = 5;
	mousex_ = 0;
	mousey_ = 0;
	t_f = true;

}

CMFC_SDIView::~CMFC_SDIView()
{
}

//��������� ������ ����
BOOL CMFC_SDIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ������� ������������� ���������� 
	//����� ��������� ���� ����������
	     // TODO: Modify the Window class or styles here by modifying
		  //  the CREATESTRUCT cs
		  // ����������������������:
		cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return CView::PreCreateWindow(cs);

}


void CMFC_SDIView::InitViewport(int x, int y, int width, int height)
{
	// ������������� ���� ��������� � ��������� ����
	glViewport(0, 0, width, height);

	// ������������������ ����� ��� ���������� ���������
	GLfloat aspectRatio = (GLfloat)width / (GLfloat)height;// ��� ���������
																   // ������������� ������� �������������� � ����� �������������
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// ������������� ������������� ��������
	
	//gluPerspective(35.0f, aspectRatio, 0.001f, 100.0f);
    gluPerspective(45.0f, 1.0f, 1.0f, 1000.0f);

	

	// ��������������� ������� �������������� � �������� ����� ����
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


// ��������� CMFC_SDIView

void CMFC_SDIView::OnDraw(CDC* /*pDC*/)
{
	CMFC_SDIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: add draw code for native data here
		CRect ClientRect;	// ��������� ���������� ���� �������������
	GetClientRect(&ClientRect);       // ���������� ����� � ������ ����
	windW = abs(ClientRect.right - ClientRect.left);
	windH = abs(ClientRect.top - ClientRect.bottom);
	
	InitViewport(0, 0, windW, windH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glRotatef(alpha_, 1., 0., 0.);	// ������� ������ ��� x �� ���� al-pha_
	glRotatef(beta_, 0., 1., 0.);	// ������� ������ ��� y �� ���� be-ta_
	
	if (t_f_specular == true)
	{
		SetupRC1();   //��������� ������ � ������
	}
	else
		SetupRC1();   //��������� ������ � ������
	
    RenderScene1();
	SetupRC1();

	//scena2();
	
		//glFlush();
	//SwapBuffers(wglGetCurrentDC());


	// TODO: �������� ����� ��� ��������� ��� ����������� ������
}


void CMFC_SDIView::DrawModels()
{
    // ��������� ���������, �� ������� �������� �������
    glColor3f(0.0f, 0.0f, 0.90f); // Blue
    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -25.0f, -100.0f);
    glVertex3f(-100.0f, -25.0f, 100.0f);
    glVertex3f(100.0f, -25.0f, 100.0f);
    glVertex3f(100.0f, -25.0f, -100.0f);
    glEnd();

    // ��������� �������� ����
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidCube(48.0f);

    // ��������� ������� �����
    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-60.0f, 0.0f, 0.0f);
    glutSolidSphere(25.0f, 50, 50);
    glPopMatrix();

    // ��������� ���������� ������
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(60.0f, 0.0f, -24.0f);
    glutSolidCone(25.0f, 50.0f, 50, 50);
    glPopMatrix();

    // ��������� ����
    glColor3f(1.0f, 0.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 60.0f);
    glutSolidTorus(8.0f, 16.0f, 50, 50);
    glPopMatrix();

    // ��������� ��������������
    glColor3f(0.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -60.0f);
    glScalef(25.0f, 25.0f, 25.0f);
    glutSolidOctahedron();
    glPopMatrix();
}

// ���������� ��� �������������� ����� �����
void CMFC_SDIView::RegenerateShadowMap()
{
    GLfloat lightToSceneDistance, nearPlane, fieldOfView;
    GLfloat lightModelview[16], lightProjection[16];

    // ���������� �������� ��������� ��� ��� �����
    lightToSceneDistance = sqrt(lightPos[0] * lightPos[0] + lightPos[1] * lightPos[1] + lightPos[2] * lightPos[2]);
    nearPlane = lightToSceneDistance - 150.0f;
    if (nearPlane < 50.0f)
        nearPlane = 50.0f;

    // � ������� ����� ��������� ������� �������
    fieldOfView = 17000.0f / lightToSceneDistance;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fieldOfView, 1.0f, nearPlane, nearPlane + 300.0f);
    glGetFloatv(GL_PROJECTION_MATRIX, lightProjection);

    // ������������ �� ����� ������ �����
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(lightPos[0], lightPos[1], lightPos[2],
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightModelview);
    glViewport(0, 0, shadowSize, shadowSize);

    // ������� ���� ������� ������ �������
    glClear(GL_DEPTH_BUFFER_BIT);

    // ���, ��� ��� ����� �������, - ��� �������������� �������� �������
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_NORMALIZE);
    glColorMask(0, 0, 0, 0);

    // ����������� �����������
    glEnable(GL_POLYGON_OFFSET_FILL);

    // ��������� �������� �����
    DrawModels();

    // ����������� �������� ������� � �������� �������
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        0, 0, shadowSize, shadowSize, 0);

    // �������������� ����������� ��������� �������
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glColorMask(1, 1, 1, 1);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // ��������� ������� ������� ��� �������� ������� �����
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0.5f, 0.5f, 0.5f);
    glScalef(0.5f, 0.5f, 0.5f);
    glMultMatrixf(lightProjection);
    glMultMatrixf(lightModelview);
}


// ����������, ����� ���������� �����
void CMFC_SDIView::RenderScene1()
{
    // ���� ������� ������ ��������
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2],
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glViewport(0, 0, windowWidth, windowHeight);

    // ��������� ��������� �����
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // ������� ���� ������� ������ �������
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (showShadowMap)
    {
        // ����������� ������� ����� � ��������������� �����
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // �������� ����� ����� � �� ����������� ������� ������������ ����
        glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(((GLfloat)shadowSize / (GLfloat)windowWidth) * 2.0 - 1.0f,-1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(((GLfloat)shadowSize / (GLfloat)windowWidth) * 2.0 - 1.0f,
            ((GLfloat)shadowSize / (GLfloat)windowHeight) * 2.0 - 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f,((GLfloat)shadowSize / (GLfloat)windowHeight) * 2.0 - 1.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        gluPerspective(45.0f, 1.0f, 1.0f, 1000.0f);
        glMatrixMode(GL_MODELVIEW);
    }
    else if (noShadows)
    {
        // ��������� �������� ���������
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

        // ��������� �����
        DrawModels();
    }
    else
    {
        GLfloat sPlane[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        GLfloat tPlane[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
        GLfloat rPlane[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
        GLfloat qPlane[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

        if (!ambientShadowAvailable)
        {
            GLfloat lowAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
            GLfloat lowDiffuse[4] = { 0.35f, 0.35f, 0.35f, 1.0f };

            // ������ ��� ��� ������� ��������� ��� "ambient"
            // �������� ������ ��������� �����, ������� ��� 
            // �������� ���������� ���������� ������...
            glLightfv(GL_LIGHT0, GL_AMBIENT, lowAmbient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, lowDiffuse);

            // ������ ������� �����
            DrawModels();

            // �������� �����-����, ����� ��������� ���������� ���������
            glAlphaFunc(GL_GREATER, 0.9f);
            glEnable(GL_ALPHA_TEST);
        }

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

        // ��������� ��������� �����
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // ���������  ��������� ����� ��� ������������� ����� ����� �� �����
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        glEnable(GL_TEXTURE_GEN_Q);
        glTexGenfv(GL_S, GL_EYE_PLANE, sPlane);
        glTexGenfv(GL_T, GL_EYE_PLANE, tPlane);
        glTexGenfv(GL_R, GL_EYE_PLANE, rPlane);
        glTexGenfv(GL_Q, GL_EYE_PLANE, qPlane);

        // ������ ������� �����
        DrawModels();

        glDisable(GL_ALPHA_TEST);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
        glDisable(GL_TEXTURE_GEN_Q);
    }

    if (glGetError() != GL_NO_ERROR)
        fprintf(stderr, "GL Error!\n");

    // ������� ��� ������������
    glFlush();
    SwapBuffers(wglGetCurrentDC());
}


// ��� ������� ��������� ����������� ������������� ��� ����������

void CMFC_SDIView::SetupRC1()
{
    const GLubyte* version;

    // ������ ���
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // �������� ������� �����������
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glPolygonOffset(factor, 0.0f);

    // ��������� ���������� ��������� ���������, �������  �� ��������
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);

    // ��������� ��������� ��������, �������  �� ��������
    glGenTextures(1, &shadowTextureID);
    glBindTexture(GL_TEXTURE_2D, shadowTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    
    if (ambientShadowAvailable)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB,0.5f);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

    RegenerateShadowMap();
}



// ������ CMFC_SDIView

BOOL CMFC_SDIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// ���������� �� ���������
	return DoPreparePrinting(pInfo);
}

void CMFC_SDIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �������� �������������� ������������� ����� �������
}

void CMFC_SDIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �������� ������� ����� ������
}


// ����������� CMFC_SDIView

#ifdef _DEBUG
void CMFC_SDIView::AssertValid() const
{
	CView::AssertValid();
}

void CMFC_SDIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFC_SDIDoc* CMFC_SDIView::GetDocument() const // �������� ������������ ������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFC_SDIDoc)));
	return (CMFC_SDIDoc*)m_pDocument;
}
#endif //_DEBUG


// ����������� ��������� CMFC_SDIView


int CMFC_SDIView::OnCreate(LPCREATESTRUCT lpCreateStruct)

	// ������� ��������� ��������� ��������� �����
	{     if (CView::OnCreate(lpCreateStruct)==-1)
		return -1;
	// TODO: Add your specialized creation code here

	// ����������� ������ �������� ��� ������ � OpenGL:
	PIXELFORMATDESCRIPTOR pfd;      // ��������� ��� �������� ������� ��������
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // ��� ������ ��������� ��� �������� ��������� ������
													// �������� ���������� ������ ��� �������� ���������:
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // ������ ���������, ������������� ������ ������� ����PIXELFORMATDESCRIPTOR
	pfd.nVersion = 1;                     	// ����� ������, ��� ������ � OpenGL ������ 1
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |       // �������� � ����
		PFD_SUPPORT_OPENGL |                 // ��������� OpenGL
		PFD_DOUBLEBUFFER;                      // ������������������
	pfd.iPixelType = PFD_TYPE_RGBA;     // �������������� RGBA
	pfd.cColorBits = 32;                                // 32-�������������������
	pfd.cAlphaBits = 8;                                 // ��������������������
	pfd.cAccumBits = 32;                             // ����� ������������
	pfd.cDepthBits = 32;                              // ����� ������� (z-�����)
	pfd.cStencilBits = 8;                              // ��������
	pfd.iLayerType = PFD_MAIN_PLANE;   // ��� �����������������
	m_pdc = GetDC();                           // ������� �������� ����������
											   // ������ �� ��������� �������� � ���������� ��������� ����������, �������� �����������, �������� ����������� � ���������, ������ ��������:
	int nPixelFormat = ChoosePixelFormat(m_pdc->m_hDC, &pfd);
	if (nPixelFormat == 0)
	{
		MessageBox(LPCTSTR("������������������������������"));
		return FALSE;
	}
	BOOL bResult = SetPixelFormat(m_pdc->m_hDC, nPixelFormat, &pfd);
	// ������������� ���������� ������ ��������:
	if (!bResult)
	{
		MessageBox(LPCTSTR("������ ��� ��������� ������� ��������"));
		return FALSE;
	}
	if ((hrc = wglCreateContext(m_pdc->m_hDC)) == NULL)  // �� ������ ��������� ���������� ������� �������� ���������������
		return FALSE;
	if (wglMakeCurrent(m_pdc->m_hDC, hrc) == FALSE) // ������  ���  �������
		return FALSE;
	return 0;

}


void CMFC_SDIView::OnDestroy()
{
	// ������� ��������� ��������� ������������ �����
	{     // TODO: Add your message handler code here
		  // ������� �� �������� � ������� �������� ���������������:
		if (hrc == ::wglGetCurrentContext())     // ����������� ������� �������� ���������������
		{
			wglMakeCurrent(NULL, NULL);         // ������ ��� �� �������
			wglDeleteContext(hrc);        // ������� �������� ���������������
			hrc = NULL;
		}
		CView::OnDestroy();                   // ���������� ���������� ������
	}

}

void CMFC_SDIView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �������� ���� ��� ����������� ��������� ��� ����� ������������
	if (nFlags&MK_LBUTTON)
	{

		// ��������� ����������alpha_� beta_������� ����� ������ � ������� ����������� ����

		beta_ = (point.x - mousex_);
		alpha_ =(point.y - mousey_);
		xPos = point.x; yPos = point.y;
		Invalidate(false);

	}

	CView::OnMouseMove(nFlags, point);
}



void CMFC_SDIView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �������� ���� ��� ����������� ��������� ��� ����� ������������
	xPos = point.x; yPos = point.y;
	mousex_ = point.x;
	mousey_ = point.y;
	
	Invalidate(false);

	CView::OnLButtonDown(nFlags, point);
}


void CMFC_SDIView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �������� ���� ��� ����������� ��������� ��� ����� ������������
	RenderScene1();  //�����-���  ����� �����
	SetupRC1();

	CView::OnTimer(nIDEvent);
}

void CMFC_SDIView::OnButton32771()
{
	// TODO: �������� ���� ��� ����������� ������
	// TODO: �������� ���� ��� ����������� ������
	// TODO: �������� ���� ��� ����������� ������

	if (t_f == true)				   // ���� ������ �� ����������
	{
		t_f = false;			   // ���������� ��� (���������)

		m_nTimer = SetTimer(1, 1, 0);  // � ������ ���������
	}
	else					   // � ����� (����������)
	{
		t_f = true;			   // ����� ���������

		KillTimer(m_nTimer);    // ���������	

	}
}



void CMFC_SDIView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �������� ���� ��� ����������� ��������� ��� ����� ������������

	CView::OnChar(nChar, nRepCnt, nFlags);
}


void CMFC_SDIView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �������� ���� ��� ����������� ��������� ��� ����� ������������

	if (nChar == VK_UP) //�����
	{
		gltMoveFrameForward(&frameCamera, -0.1f);
		
		RenderScene1();  //�����-���  ����� �����
		SetupRC1();
		//
		//					//RenderSceneSphereWorld();
		//
			glFlush();
			SwapBuffers(wglGetCurrentDC());
	}

	if (nChar == VK_DOWN) //�����
	{

		gltMoveFrameForward(&frameCamera, 0.1f);
		RenderScene1();  //�����-���  ����� �����
		SetupRC1();

		glFlush();
		SwapBuffers(wglGetCurrentDC());
	}


	if (nChar == VK_LEFT) //�����
	{
		gltRotateFrameLocalY(&frameCamera, 0.1);
		RenderScene();
		glFlush();
		SwapBuffers(wglGetCurrentDC());


	}
	if (nChar == VK_RIGHT) //������
	{
		gltRotateFrameLocalY(&frameCamera, -0.1);
		RenderScene();
		glFlush();
		SwapBuffers(wglGetCurrentDC());

	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CMFC_SDIView::OnButton32777()
{
	// TODO: �������� ���� ��� ����������� ������
	// TODO: �������� ���� ��� ����������� ������
	// TODO: �������� ���� ��� ����������� ������
	// TODO: �������� ���� ��� ����������� ������

	if (t_f_specular == true)				   // ���� ������ �� ����������
	{
		t_f_specular = false;			   // ���������� ��� (���������)

		
	}
	else					   // � ����� (����������)
	{
		t_f_specular = true;			   // ����� ���������

			

	}
}
