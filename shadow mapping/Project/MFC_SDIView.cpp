// MFC_SDIView.cpp : реализация класса CMFC_SDIView
//

#include "stdafx.h"
// SHARED_HANDLERS можно определить в обработчиках фильтров просмотра реализации проекта ATL, эскизов
// и поиска; позволяет совместно использовать код документа в данным проекте.
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
	// Стандартные команды печати
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

//Описание переменных
GLboolean ambientShadowAvailable = GL_FALSE;
GLboolean controlCamera = GL_TRUE;      // клавиши xyz будут управлять lightpos
GLboolean noShadows = GL_FALSE;         // нормальное освещение
GLboolean showShadowMap = GL_FALSE;     // показать текстуру теневой карты

GLfloat factor = 4.0f;                  // для смещения полигонов

GLint windowWidth = 512;                // Размер окна
GLint windowHeight = 512;

GLint shadowSize = 512;                 // устанавливается в зависимости от размера окна
GLuint shadowTextureID;

GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat noLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightPos[] = { 100.0f, 300.0f, 100.0f, 1.0f };
GLfloat cameraPos[] = { 100.0f, 150.0f, 200.0f, 1.0f };



//Конструктор класса
CMFC_SDIView::CMFC_SDIView()
{
	// TODO: добавьте код создания
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

//Установка стилей окна
BOOL CMFC_SDIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// функция автоматически вызывается 
	//перед созданием окна приложения
	     // TODO: Modify the Window class or styles here by modifying
		  //  the CREATESTRUCT cs
		  // устанавливаемстилиокна:
		cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return CView::PreCreateWindow(cs);

}


void CMFC_SDIView::InitViewport(int x, int y, int width, int height)
{
	// Устанавливаем поле просмотра с размерами окна
	glViewport(0, 0, width, height);

	// Характеристическое число для соблюдения пропорций
	GLfloat aspectRatio = (GLfloat)width / (GLfloat)height;// Для коррекции
																   // Устанавливает матрицу преобразования в режим проецирования
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Устанавливаем перспективную проекцию
	
	//gluPerspective(35.0f, aspectRatio, 0.001f, 100.0f);
    gluPerspective(45.0f, 1.0f, 1.0f, 1000.0f);

	

	// Восстановливает матрицу преобразования в исходный режим вида
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


// рисование CMFC_SDIView

void CMFC_SDIView::OnDraw(CDC* /*pDC*/)
{
	CMFC_SDIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: add draw code for native data here
		CRect ClientRect;	// описываем переменную типа прямоугольник
	GetClientRect(&ClientRect);       // определяем длину и ширину окна
	windW = abs(ClientRect.right - ClientRect.left);
	windH = abs(ClientRect.top - ClientRect.bottom);
	
	InitViewport(0, 0, windW, windH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glRotatef(alpha_, 1., 0., 0.);	// поворот вокруг оси x на угол al-pha_
	glRotatef(beta_, 0., 1., 0.);	// поворот вокруг оси y на угол be-ta_
	
	if (t_f_specular == true)
	{
		SetupRC1();   //Установка камеры и фрейма
	}
	else
		SetupRC1();   //Установка камеры и фрейма
	
    RenderScene1();
	SetupRC1();

	//scena2();
	
		//glFlush();
	//SwapBuffers(wglGetCurrentDC());


	// TODO: добавьте здесь код отрисовки для собственных данных
}


void CMFC_SDIView::DrawModels()
{
    // Рисование плоскости, на которой покоятся объекты
    glColor3f(0.0f, 0.0f, 0.90f); // Blue
    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -25.0f, -100.0f);
    glVertex3f(-100.0f, -25.0f, 100.0f);
    glVertex3f(100.0f, -25.0f, 100.0f);
    glVertex3f(100.0f, -25.0f, -100.0f);
    glEnd();

    // Рисование красного куба
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidCube(48.0f);

    // Рисование зеленой сферы
    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-60.0f, 0.0f, 0.0f);
    glutSolidSphere(25.0f, 50, 50);
    glPopMatrix();

    // Рисование оранжевого конуса
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(60.0f, 0.0f, -24.0f);
    glutSolidCone(25.0f, 50.0f, 50, 50);
    glPopMatrix();

    // Рисование тора
    glColor3f(1.0f, 0.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 60.0f);
    glutSolidTorus(8.0f, 16.0f, 50, 50);
    glPopMatrix();

    // Рисование восьмигранника
    glColor3f(0.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -60.0f);
    glScalef(25.0f, 25.0f, 25.0f);
    glutSolidOctahedron();
    glPopMatrix();
}

// Вызывается для восстановления карты теней
void CMFC_SDIView::RegenerateShadowMap()
{
    GLfloat lightToSceneDistance, nearPlane, fieldOfView;
    GLfloat lightModelview[16], lightProjection[16];

    // Записываем точность измерения где это нужно
    lightToSceneDistance = sqrt(lightPos[0] * lightPos[0] + lightPos[1] * lightPos[1] + lightPos[2] * lightPos[2]);
    nearPlane = lightToSceneDistance - 150.0f;
    if (nearPlane < 50.0f)
        nearPlane = 50.0f;

    // С помощью сцены заполняем тестуру глубины
    fieldOfView = 17000.0f / lightToSceneDistance;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fieldOfView, 1.0f, nearPlane, nearPlane + 300.0f);
    glGetFloatv(GL_PROJECTION_MATRIX, lightProjection);

    // Переключение на точку зрения света
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(lightPos[0], lightPos[1], lightPos[2],
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightModelview);
    glViewport(0, 0, shadowSize, shadowSize);

    // Очищаем окно текущим цветом очистки
    glClear(GL_DEPTH_BUFFER_BIT);

    // Все, что нас здесь волнует, - это результирующие значения глубины
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_NORMALIZE);
    glColorMask(0, 0, 0, 0);

    // Преодоление неточностей
    glEnable(GL_POLYGON_OFFSET_FILL);

    // Рисование объектов сцены
    DrawModels();

    // Копирование значения глубины в текстуру глубины
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        0, 0, shadowSize, shadowSize, 0);

    // Восстановление нормального состояния чертежа
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glColorMask(1, 1, 1, 1);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Настройка матрицы текстур для проекции теневой карты
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0.5f, 0.5f, 0.5f);
    glScalef(0.5f, 0.5f, 0.5f);
    glMultMatrixf(lightProjection);
    glMultMatrixf(lightModelview);
}


// Вызывается, чтобы нарисовать сцену
void CMFC_SDIView::RenderScene1()
{
    // Угол наклона камеры слежения
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2],
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glViewport(0, 0, windowWidth, windowHeight);

    // Положение подсветки трека
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Очистка окна текущим цветом очистки
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (showShadowMap)
    {
        // Отображение теневой карты в образовательных целях
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

        // Показать карту теней в ее фактическом размере относительно окна
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
        // Установки простого освещения
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

        // Рисование сцены
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

            // Потому что нет никакой поддержки для "ambient"
            // значение ошибки сравнения теней, сначала нам 
            // придется нарисовать окружающий проход...
            glLightfv(GL_LIGHT0, GL_AMBIENT, lowAmbient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, lowDiffuse);

            // Рисуем объекты сцены
            DrawModels();

            // Включаем альфа-тест, чтобы отбросить затененные фрагменты
            glAlphaFunc(GL_GREATER, 0.9f);
            glEnable(GL_ALPHA_TEST);
        }

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

        // Настройка сравнения теней
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Настройка  плоскости глаза для проецирования карты теней на сцену
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        glEnable(GL_TEXTURE_GEN_Q);
        glTexGenfv(GL_S, GL_EYE_PLANE, sPlane);
        glTexGenfv(GL_T, GL_EYE_PLANE, tPlane);
        glTexGenfv(GL_R, GL_EYE_PLANE, rPlane);
        glTexGenfv(GL_Q, GL_EYE_PLANE, qPlane);

        // Рисуем объекты сцены
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

    // Команда для перереисовки
    glFlush();
    SwapBuffers(wglGetCurrentDC());
}


// Эта функция выполняет необходимую инициализацию при рендеринге

void CMFC_SDIView::SetupRC1()
{
    const GLubyte* version;

    // Черный фон
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Удаление скрытой поверхности
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glPolygonOffset(factor, 0.0f);

    // Настройка некоторого состояние освещения, которое  не меняется
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);

    // Настройка состояния текстуры, которое  не меняется
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



// печать CMFC_SDIView

BOOL CMFC_SDIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// подготовка по умолчанию
	return DoPreparePrinting(pInfo);
}

void CMFC_SDIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте дополнительную инициализацию перед печатью
}

void CMFC_SDIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте очистку после печати
}


// диагностика CMFC_SDIView

#ifdef _DEBUG
void CMFC_SDIView::AssertValid() const
{
	CView::AssertValid();
}

void CMFC_SDIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFC_SDIDoc* CMFC_SDIView::GetDocument() const // встроена неотлаженная версия
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFC_SDIDoc)));
	return (CMFC_SDIDoc*)m_pDocument;
}
#endif //_DEBUG


// обработчики сообщений CMFC_SDIView


int CMFC_SDIView::OnCreate(LPCREATESTRUCT lpCreateStruct)

	// функция обработки сообщения “Создание окна”
	{     if (CView::OnCreate(lpCreateStruct)==-1)
		return -1;
	// TODO: Add your specialized creation code here

	// Настраиваем формат пикселей для работы с OpenGL:
	PIXELFORMATDESCRIPTOR pfd;      // структура для описания формата пикселей
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // для начала заполняем все значения структуры нулями
													// Начинаем заполнение нужных нам значений структуры:
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // размер структуры, устанавливаем равным размеру типаPIXELFORMATDESCRIPTOR
	pfd.nVersion = 1;                     	// номер версии, для работы с OpenGL ставим 1
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |       // рисовать в окне
		PFD_SUPPORT_OPENGL |                 // поддержка OpenGL
		PFD_DOUBLEBUFFER;                      // двойнаябуферизация
	pfd.iPixelType = PFD_TYPE_RGBA;     // цветоваямодель RGBA
	pfd.cColorBits = 32;                                // 32-битоваяглубинацвета
	pfd.cAlphaBits = 8;                                 // компонентаальфацвета
	pfd.cAccumBits = 32;                             // буфер аккумулятора
	pfd.cDepthBits = 32;                              // буфер глубины (z-буфер)
	pfd.cStencilBits = 8;                              // трафарет
	pfd.iLayerType = PFD_MAIN_PLANE;   // тип плоскости–основная
	m_pdc = GetDC();                           // создаем контекст устройства
											   // Исходя из настройки пикселей и созданного контекста устройства, получаем оптимальный, наиболее совпадающий с требуемым, формат пикселей:
	int nPixelFormat = ChoosePixelFormat(m_pdc->m_hDC, &pfd);
	if (nPixelFormat == 0)
	{
		MessageBox(LPCTSTR("Ошибкапривыбореформатапикселей"));
		return FALSE;
	}
	BOOL bResult = SetPixelFormat(m_pdc->m_hDC, nPixelFormat, &pfd);
	// Устанавливаем полученный формат пикселей:
	if (!bResult)
	{
		MessageBox(LPCTSTR("Ошибка при установке формата пикселей"));
		return FALSE;
	}
	if ((hrc = wglCreateContext(m_pdc->m_hDC)) == NULL)  // на основе контекста устройства создаем контекст воспроизведения
		return FALSE;
	if (wglMakeCurrent(m_pdc->m_hDC, hrc) == FALSE) // делаем  его  текущим
		return FALSE;
	return 0;

}


void CMFC_SDIView::OnDestroy()
{
	// функция обработки сообщения “Уничтожение окна”
	{     // TODO: Add your message handler code here
		  // Убираем из текущего и удаляем контекст воспроизведения:
		if (hrc == ::wglGetCurrentContext())     // запрашиваем текущий контекст воспроизведения
		{
			wglMakeCurrent(NULL, NULL);         // делаем его не текущим
			wglDeleteContext(hrc);        // удаляем контекст воспроизведения
			hrc = NULL;
		}
		CView::OnDestroy();                   // корректное завершение работы
	}

}

void CMFC_SDIView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	if (nFlags&MK_LBUTTON)
	{

		// прибавить переменнымalpha_и beta_разницу между новыми и старыми положениями мыши

		beta_ = (point.x - mousex_);
		alpha_ =(point.y - mousey_);
		xPos = point.x; yPos = point.y;
		Invalidate(false);

	}

	CView::OnMouseMove(nFlags, point);
}



void CMFC_SDIView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	xPos = point.x; yPos = point.y;
	mousex_ = point.x;
	mousey_ = point.y;
	
	Invalidate(false);

	CView::OnLButtonDown(nFlags, point);
}


void CMFC_SDIView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	RenderScene1();  //Сцена-тор  сфера земля
	SetupRC1();

	CView::OnTimer(nIDEvent);
}

void CMFC_SDIView::OnButton32771()
{
	// TODO: добавьте свой код обработчика команд
	// TODO: добавьте свой код обработчика команд
	// TODO: добавьте свой код обработчика команд

	if (t_f == true)				   // если таймер не установлен
	{
		t_f = false;			   // установить его (логически)

		m_nTimer = SetTimer(1, 1, 0);  // а теперь физически
	}
	else					   // а иначе (установлен)
	{
		t_f = true;			   // убить логически

		KillTimer(m_nTimer);    // физически	

	}
}



void CMFC_SDIView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	CView::OnChar(nChar, nRepCnt, nFlags);
}


void CMFC_SDIView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	if (nChar == VK_UP) //Вверх
	{
		gltMoveFrameForward(&frameCamera, -0.1f);
		
		RenderScene1();  //Сцена-тор  сфера земля
		SetupRC1();
		//
		//					//RenderSceneSphereWorld();
		//
			glFlush();
			SwapBuffers(wglGetCurrentDC());
	}

	if (nChar == VK_DOWN) //Вверх
	{

		gltMoveFrameForward(&frameCamera, 0.1f);
		RenderScene1();  //Сцена-тор  сфера земля
		SetupRC1();

		glFlush();
		SwapBuffers(wglGetCurrentDC());
	}


	if (nChar == VK_LEFT) //Влево
	{
		gltRotateFrameLocalY(&frameCamera, 0.1);
		RenderScene();
		glFlush();
		SwapBuffers(wglGetCurrentDC());


	}
	if (nChar == VK_RIGHT) //Вправо
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
	// TODO: добавьте свой код обработчика команд
	// TODO: добавьте свой код обработчика команд
	// TODO: добавьте свой код обработчика команд
	// TODO: добавьте свой код обработчика команд

	if (t_f_specular == true)				   // если таймер не установлен
	{
		t_f_specular = false;			   // установить его (логически)

		
	}
	else					   // а иначе (установлен)
	{
		t_f_specular = true;			   // убить логически

			

	}
}
