
// MFC_SDIView.h : ��������� ������ CMFC_SDIView
//

#pragma once


class CMFC_SDIView : public CView
{
protected: // ������� ������ �� ������������
	CMFC_SDIView();
	DECLARE_DYNCREATE(CMFC_SDIView)

// ��������
public:
	CMFC_SDIDoc* GetDocument() const;

// ��������

public:
	HGLRC hrc;
	CDC* m_pdc;
	int windH;
	int windW;

	GLdouble scale_;		// ��������������� �����
	GLfloat alpha_;			// ���� �������� ������ ������ ���
	GLfloat beta_;			// ���� �������� ������ ������ ���
	int mousex_;			// ��� ����������� x-���������� ����
	int mousey_;			// ��� ����������� y-���������� ����

	int xPos, yPos;
	bool t_f,t_f_specular;
	UINT_PTR m_nTimer;


#define GLT_PI	3.14159265358979323846
#define GLT_PI_DIV_180 0.017453292519943296
#define GLT_INV_PI_DIV_180 57.2957795130823229
#define gltDegToRad(x)	((x)*GLT_PI_DIV_180)
#define NUM_SPHERES      50 // ���������� ����

	typedef GLfloat GLTVector2[2];      // Two component floating point vector
	typedef GLfloat GLTVector3[3];      // Three component floating point vector
	typedef GLfloat GLTVector4[4];      // Four component floating point vector
	typedef GLfloat GLTMatrix[16];      // ������� 4x4
	typedef struct {                     // ����� 3x3
		GLTVector3 vLocation;
		GLTVector3 vUp;
		GLTVector3 vForward;
	} GLTFrame;
	GLTFrame    spheres[NUM_SPHERES];
	GLTFrame    frameCamera;
	GLTMatrix mShadowMatrix;


// ���������������
public:
	virtual void OnDraw(CDC* pDC);  // �������������� ��� ��������� ����� �������������
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ����������
public:
	virtual ~CMFC_SDIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ��������� ������� ����� ���������
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	void InitViewport(int x, int y, int width, int height);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	//int DrawGLScene(GLvoid);
	//void DrawGLRoom();
	//void VMatMult(GLmatrix16f M, GLvector4f v);
	//GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
	//int InitGLObjects();
	//int InitGL(GLvoid);
	//void ProcessKeyboard();

	void DrawModels();
	void RegenerateShadowMap();
	void RenderScene1();
	void SetupRC1();

	void SetupRC(); void SetupRC_specular();
	void DrawGround(); void DrawGround_specular();
	void DrawInhabitants(GLint nShadow);
	void RenderScene();
	void gltDrawTorus(GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
	void gltSubtractVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult);
	void gltVectorCrossProduct(const GLTVector3 vU, const GLTVector3 vV, GLTVector3 vResult);
	void gltGetNormalVector(const GLTVector3 vP1, const GLTVector3 vP2, const GLTVector3 vP3, GLTVector3 vNormal);
	void gltGetPlaneEquation(GLTVector3 vPoint1, GLTVector3 vPoint2, GLTVector3 vPoint3, GLTVector3 vPlane);
	void gltMakeShadowMatrix(GLTVector3 vPoints[3], GLTVector4 vLightPos, GLTMatrix destMat);
	void gltScaleVector(GLTVector3 vVector, const GLfloat fScale);
	
	GLfloat gltGetVectorLength(const GLTVector3 vVector);
	void gltNormalizeVector(GLTVector3 vNormal);
	
	void Stena(), Zad(), Left(), Right();
	

	void scena2();
	void SphereWorld(void);
	void RenderSceneSphereWorld();
	//void gltApplyCameraTransform(&frameCamera);
	void gltApplyCameraTransform(GLTFrame* pCamera);
void ChangeSizeSphereWorld(int width, int height);
void gltApplyActorTransform(GLTFrame* pFrame);
GLfloat gltGetVectorLengthSqrd(const GLTVector3 vVector);
void gltInitFrame(GLTFrame* pFrame);
void gltMoveFrameForward(GLTFrame* pFrame, GLfloat fStep);
void gltRotateFrameLocalY(GLTFrame* pFrame, GLfloat fAngle);
void SetupSphereWorld(void);




	void gltTransformPoint(const GLTVector3 vSrcPoint, const GLTMatrix mMatrix, GLTVector3 vPointOut);
	void gltRotateVector(const GLTVector3 vSrcVector, const GLTMatrix mMatrix, GLTVector3 vPointOut);
	void gltLoadIdentityMatrix(GLTMatrix m);
	void gltMultiplyMatrix(const GLTMatrix m1, const GLTMatrix m2, GLTMatrix mProduct);
	void gltRotationMatrix(float angle, float x, float y, float z, GLTMatrix mMatrix);
	void gltTranslationMatrix(GLfloat x, GLfloat y, GLfloat z, GLTMatrix mTranslate);
	void gltScalingMatrix(GLfloat x, GLfloat y, GLfloat z, GLTMatrix mScale);

	void gltTransposeMatrix(GLTMatrix mTranspose);
	void gltInvertMatrix(const GLTMatrix m, GLTMatrix mInverse);
	void gltGetMatrixFromFrame(GLTFrame *pFrame, GLTMatrix mMatrix);
	void CMFC_SDIView::ChangeSize(int w, int h);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnButton32771();
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButton32777();
};

#ifndef _DEBUG  // ���������� ������ � MFC_SDIView.cpp
inline CMFC_SDIDoc* CMFC_SDIView::GetDocument() const
   { return reinterpret_cast<CMFC_SDIDoc*>(m_pDocument); }
#endif

