#pragma once

// Light and material Data
GLfloat fLightPos[4] = { -100.0f, 100.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };


// Subtract one vector from another
void CMFC_SDIView::gltSubtractVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult)
{
	vResult[0] = vFirst[0] - vSecond[0];
	vResult[1] = vFirst[1] - vSecond[1];
	vResult[2] = vFirst[2] - vSecond[2];
}


// Given three points on a plane in counter clockwise order, calculate the unit normal
void CMFC_SDIView::gltGetNormalVector(const GLTVector3 vP1, const GLTVector3 vP2, const GLTVector3 vP3, GLTVector3 vNormal)
{
	GLTVector3 vV1, vV2;

	gltSubtractVectors(vP2, vP1, vV1);
	gltSubtractVectors(vP3, vP1, vV2);

	gltVectorCrossProduct(vV1, vV2, vNormal);
	gltNormalizeVector(vNormal);
}


// Gets the three coefficients of a plane equation given three points on the plane.
void CMFC_SDIView::gltGetPlaneEquation(GLTVector3 vPoint1, GLTVector3 vPoint2, GLTVector3 vPoint3, GLTVector3 vPlane)
{
	// Get normal vector from three points. The normal vector is the first three coefficients
	// to the plane equation...
	gltGetNormalVector(vPoint1, vPoint2, vPoint3, vPlane);

	// Final coefficient found by back substitution
	vPlane[3] = -(vPlane[0] * vPoint3[0] + vPlane[1] * vPoint3[1] + vPlane[2] * vPoint3[2]);
}



// Creates a shadow projection matrix out of the plane equation
// coefficients and the position of the light. The return value is stored
// in destMat
void CMFC_SDIView::gltMakeShadowMatrix(GLTVector3 vPoints[3], GLTVector4 vLightPos, GLTMatrix destMat)
{
	GLTVector4 vPlaneEquation;
	GLfloat dot;

	gltGetPlaneEquation(vPoints[0], vPoints[1], vPoints[2], vPlaneEquation);

	// Dot product of plane and light position
	dot = vPlaneEquation[0] * vLightPos[0] +
		vPlaneEquation[1] * vLightPos[1] +
		vPlaneEquation[2] * vLightPos[2] +
		vPlaneEquation[3] * vLightPos[3];


	// Now do the projection
	// First column
	destMat[0] = dot - vLightPos[0] * vPlaneEquation[0];
	destMat[4] = 0.0f - vLightPos[0] * vPlaneEquation[1];
	destMat[8] = 0.0f - vLightPos[0] * vPlaneEquation[2];
	destMat[12] = 0.0f - vLightPos[0] * vPlaneEquation[3];

	// Second column
	destMat[1] = 0.0f - vLightPos[1] * vPlaneEquation[0];
	destMat[5] = dot - vLightPos[1] * vPlaneEquation[1];
	destMat[9] = 0.0f - vLightPos[1] * vPlaneEquation[2];
	destMat[13] = 0.0f - vLightPos[1] * vPlaneEquation[3];

	// Third Column
	destMat[2] = 0.0f - vLightPos[2] * vPlaneEquation[0];
	destMat[6] = 0.0f - vLightPos[2] * vPlaneEquation[1];
	destMat[10] = dot - vLightPos[2] * vPlaneEquation[2];
	destMat[14] = 0.0f - vLightPos[2] * vPlaneEquation[3];

	// Fourth Column
	destMat[3] = 0.0f - vLightPos[3] * vPlaneEquation[0];
	destMat[7] = 0.0f - vLightPos[3] * vPlaneEquation[1];
	destMat[11] = 0.0f - vLightPos[3] * vPlaneEquation[2];
	destMat[15] = dot - vLightPos[3] * vPlaneEquation[3];
}



// Gets the length of a vector squared
GLfloat CMFC_SDIView::gltGetVectorLengthSqrd(const GLTVector3 vVector)
{
	return (vVector[0] * vVector[0]) + (vVector[1] * vVector[1]) + (vVector[2] * vVector[2]);
}


// Gets the length of a vector
GLfloat CMFC_SDIView::gltGetVectorLength(const GLTVector3 vVector)
{
	return (GLfloat)sqrt(gltGetVectorLengthSqrd(vVector));
}




void CMFC_SDIView::gltDrawTorus(GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor)
{
	GLTVector3 vNormal;
	double majorStep = 2.0f*GLT_PI / numMajor;
	double minorStep = 2.0f*GLT_PI / numMinor;
	int i, j;

	for (i = 0; i<numMajor; ++i)
	{
		double a0 = i * majorStep;
		double a1 = a0 + majorStep;
		GLfloat x0 = (GLfloat)cos(a0);
		GLfloat y0 = (GLfloat)sin(a0);
		GLfloat x1 = (GLfloat)cos(a1);
		GLfloat y1 = (GLfloat)sin(a1);

		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0; j <= numMinor; ++j)
		{
			double b = j * minorStep;
			GLfloat c = (GLfloat)cos(b);
			GLfloat r = minorRadius * c + majorRadius;
			GLfloat z = minorRadius * (GLfloat)sin(b);

			// First point
			glTexCoord2f((float)(i) / (float)(numMajor), (float)(j) / (float)(numMinor));
			vNormal[0] = x0*c;
			vNormal[1] = y0*c;
			vNormal[2] = z / minorRadius;
			gltNormalizeVector(vNormal);
			glNormal3fv(vNormal);
			glVertex3f(x0*r, y0*r, z);

			glTexCoord2f((float)(i + 1) / (float)(numMajor), (float)(j) / (float)(numMinor));
			vNormal[0] = x1*c;
			vNormal[1] = y1*c;
			vNormal[2] = z / minorRadius;
			glNormal3fv(vNormal);
			glVertex3f(x1*r, y1*r, z);
		}
		glEnd();
	}
}



void CMFC_SDIView::SetupRC()
{
	int iSphere;

	// Calculate shadow matrix
	GLTVector3 vPoints[3] = { { 0.0f, -0.4f, 0.0f },
							 { 10.0f, -0.4f, 0.0f },
							 { 5.0f, -0.4f, -5.0f } };

	// Grayish background
	glClearColor(fLowLight[0], fLowLight[1], fLowLight[2], fLowLight[3]);

	// Cull backs of polygons
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// Setup light parameters
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	gltMakeShadowMatrix(vPoints, fLightPos, mShadowMatrix);

	// Mostly use material tracking
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	gltInitFrame(&frameCamera);  // Initialize the camera

	// Randomly place the sphere inhabitants
	for (iSphere = 0; iSphere < NUM_SPHERES; iSphere++)
	{
		gltInitFrame(&spheres[iSphere]);    // Initialize the frame

		// Pick a random location between -20 and 20 at .1 increments
		spheres[iSphere].vLocation[0] = (float)((rand() % 400) - 200) * 0.1f;
		spheres[iSphere].vLocation[1] = 0.0f;
		spheres[iSphere].vLocation[2] = (float)((rand() % 400) - 200) * 0.1f;
	}
}



void CMFC_SDIView::SetupRC_specular()
{
	// Grayish background
	glClearColor(fLowLight[0], fLowLight[1], fLowLight[2], fLowLight[3]);

	// Cull backs of polygons
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// Setup light parameters
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Mostly use material tracking
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

}

void CMFC_SDIView::DrawGround_specular()
{
	GLfloat fExtent = 20.0f;
	GLfloat fStep = 0.5f;
	GLfloat y = 0.0f;
	GLfloat fColor;
	GLfloat iStrip, iRun;
	GLint iBounce = 0;

	glShadeModel(GL_FLAT);
	for (iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
		{
			if ((iBounce % 2) == 0)
				fColor = 1.0f;
			else
				fColor = 1.0f;

			glColor4f(fColor, fColor, fColor, 0.5f);
			glVertex3f(iStrip, y, iRun);
			glVertex3f(iStrip + fStep, y, iRun);

			iBounce++;
		}
		glEnd();
	}
	glShadeModel(GL_SMOOTH);

}



void CMFC_SDIView::Left()
{
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);

	glVertex3f(-7, 2, 20);
	glVertex3f(-7, -2, 20);
	

	glVertex3f(-5, -1, -10);
	glVertex3f(-5, 0.9, -10);
	
	glEnd();

}

void CMFC_SDIView::Right()
{
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);


	glVertex3f(7, -2, 20);
	glVertex3f(7, 2, 20);
	

	glVertex3f(5, 0.9, -10);
	glVertex3f(5, -1, -10);
	

	glEnd();

}

void CMFC_SDIView::Zad()
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		
		glVertex3f(-10, 1, -11);
		glVertex3f(-10, -1, -11);
		glVertex3f(10, -1, -11);
		glVertex3f(10, 1, -11);
		glEnd();
	
}


void CMFC_SDIView::Stena()
{
	GLfloat fExtent = 20.0f;
	GLfloat fStep = 1.0f;
	GLfloat y = 1.4f;
	GLint iStrip, iRun;

	glColor3f(0.9f, .20f, .10f);

	for (iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up

		for (iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
		{
			glVertex3f(iStrip, y, iRun);
			glVertex3f(iStrip - fStep, y, iRun);
		}
		glEnd();
	}
}


void CMFC_SDIView::DrawGround()
{
	GLfloat fExtent = 20.0f;
	GLfloat fStep = 1.0f;
	GLfloat y = -0.4f;
	GLint iStrip, iRun;

	for (iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up

		for (iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
		{
			glVertex3f(iStrip, y, iRun);
			glVertex3f(iStrip + fStep, y, iRun);
		}
		glEnd();
	}
}


/// <summary>
///Расположение объектов
/// </summary>
/// <param name="nShadow"></param>
void CMFC_SDIView::DrawInhabitants(GLint nShadow)
{
	static GLfloat yRot = 0.0f;         // Rotation angle for animation
	GLint i;

	if (nShadow == 0)
		yRot += 0.5f;
	else
		glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

////////////////////////////////////////////

	if (nShadow == 0)
		glColor3f(0.0f, 1.0f, 0.0f);




	//glColor3f(0.0f, 0.0f, 1.0f);
	glPushMatrix();
	//	gltApplyActorTransform(&spheres[i]);
	glTranslatef(-2.2f, 0.1f, -3.5f);
	glRotatef(-90, 1, 0, 0);

	GLUquadricObj* quadratic;
	quadratic = gluNewQuadric();

	//основн радиус  базов радиус  число слайсов  число стеков
	gluCylinder(quadratic, 0.1f, 0.1f, 3.0f, 32, 32);

	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.1f, -2.5f);


	////////////////////////////////////////////
	if (nShadow == 0)
		glColor3f(0.0f, 1.0f, 1.0f);

	glPushMatrix();
	//	gltApplyActorTransform(&spheres[i]);
	glTranslatef(-1.2f, 0.1f, -6.5f);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(0.2, 1, 10, 10); //Радиус, высота, сегм по х и сегм по у
	glPopMatrix();

/////////////////////

	if (nShadow == 0)
		glColor3f(0.0f, 1.0f, 0.0f);

	glPushMatrix();
	//	gltApplyActorTransform(&spheres[i]);
	glTranslatef(1.9f, 0.1f, -6.5f);
	glutSolidCube(1);
	glPopMatrix();

	/////////////////////////
	// Draw the randomly located spheres
	if (nShadow == 0)
		glColor3f(0.0f, 1.0f, 0.0f);

		
		glPushMatrix();
	//	gltApplyActorTransform(&spheres[i]);
		glTranslatef(-1.9f, 0.1f, -5.5f);
		glutSolidSphere(0.2f, 21, 11);  //Радиус, кол-во ободов вдоль и поперек
		glPopMatrix();
	
		
		
/////////////////////////////////////
	if (nShadow == 0)
		glColor3f(0.0f, 0.0f, 1.0f);

	glPushMatrix();
	glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(1.0f, 0.0f, 0.0f);
	glutSolidSphere(0.1f, 21, 11);
	glPopMatrix();

	if (nShadow == 0)
	{
		// Torus alone will be specular
		glColor3f(1.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
	}

	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	gltDrawTorus(0.35, 0.15, 61, 37);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
	glPopMatrix();
}


void CMFC_SDIView::RenderScene()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	
	glPushMatrix();
	gltApplyCameraTransform(&frameCamera);

	// Position light before any other transformations
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);

	// Draw the ground
	glColor3f(0.60f, .40f, .10f);
	
	if (t_f_specular = true)
	{
		DrawGround();
	}
	else
		DrawGround_specular();



	Stena();
	Zad();
	Left();
	Right();

	// Draw shadows first
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_STENCIL_TEST);
	glPushMatrix();
	glMultMatrixf(mShadowMatrix);
	DrawInhabitants(1);
	glPopMatrix();
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	// Draw inhabitants normally
	DrawInhabitants(0);

	glPopMatrix();

	glFlush();
	SwapBuffers(wglGetCurrentDC());
}

// Transform a point by a 4x4 matrix
void CMFC_SDIView::gltTransformPoint(const GLTVector3 vSrcVector, const GLTMatrix mMatrix, GLTVector3 vOut)
{
	vOut[0] = mMatrix[0] * vSrcVector[0] + mMatrix[4] * vSrcVector[1] + mMatrix[8] * vSrcVector[2] + mMatrix[12];
	vOut[1] = mMatrix[1] * vSrcVector[0] + mMatrix[5] * vSrcVector[1] + mMatrix[9] * vSrcVector[2] + mMatrix[13];
	vOut[2] = mMatrix[2] * vSrcVector[0] + mMatrix[6] * vSrcVector[1] + mMatrix[10] * vSrcVector[2] + mMatrix[14];
}


///////////////////////////////////////////////////////////////////////

//**********************************************************
// Формировать сцену
void CMFC_SDIView::SphereWorld()
{
	static GLfloat yRot = 0.0f;	// Угол поворота для анимации в градусах

	glPushMatrix();
	gltApplyCameraTransform(&frameCamera);
	// Рисуем сетку
	DrawGround();
	// Рисовать сферы по ранее сгенерированным случайным координатам
	for (int i = 0; i < NUM_SPHERES; i++)
	{
		glPushMatrix();
		gltApplyActorTransform(&spheres[i]);
		glutSolidSphere(0.1f, 13, 26);
		glPopMatrix();
	}
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -2.5f);
	glPushMatrix();
	glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(1.0f, 0.0f, 0.0f);
	glutSolidSphere(0.1f, 13, 26);
	glPopMatrix();
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	gltDrawTorus(0.35, 0.15, 40, 20);
	glPopMatrix();
	glPopMatrix();

	// Наращиваем анимацию
	yRot += 0.5f;

	// Переключаем буферы
	glFlush();
	SwapBuffers(wglGetCurrentDC());
}

//**********************************************************
// Рендеринг
void CMFC_SDIView::RenderSceneSphereWorld()
{
	// Сохраняем атрибуты в стеке атрибутов!!!
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);

	// Устанавливаем белый цвет и голубой фон
	glColor3ub(255, 255, 255); // Белый
	glClearColor(0.0f, 0.0f, .50f, 1.0f);
	// Сбрасываем буферы цвета и глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Рисовать с двух сторон в каркасном режиме
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	SphereWorld(); // Рисуем сцену

	// Восстанавливаем атрибуты из стека атрибутов!!!
	glPopAttrib();

	// Прокачка сообщений
	glFlush();
	SwapBuffers(wglGetCurrentDC());
}

//**********************************************************
//**********************************************************
// Управление фреймом камеры


//**********************************************************
// Формируем матрицу проекции
void CMFC_SDIView::gltApplyCameraTransform(GLTFrame* pCamera)
{
	GLTMatrix mMatrix;
	GLTVector3 vAxisX;
	GLTVector3 zFlipped;

	zFlipped[0] = -pCamera->vForward[0];
	zFlipped[1] = -pCamera->vForward[1];
	zFlipped[2] = -pCamera->vForward[2];

	// Получим вектор X
	gltVectorCrossProduct(pCamera->vUp, zFlipped, vAxisX);

	// Формируем матрицу поворота и транспонируем ее
	mMatrix[0] = vAxisX[0];
	mMatrix[4] = vAxisX[1];
	mMatrix[8] = vAxisX[2];
	mMatrix[12] = 0.0f;

	mMatrix[1] = pCamera->vUp[0];
	mMatrix[5] = pCamera->vUp[1];
	mMatrix[9] = pCamera->vUp[2];
	mMatrix[13] = 0.0f;

	mMatrix[2] = zFlipped[0];
	mMatrix[6] = zFlipped[1];
	mMatrix[10] = zFlipped[2];
	mMatrix[14] = 0.0f;

	mMatrix[3] = 0.0f;
	mMatrix[7] = 0.0f;
	mMatrix[11] = 0.0f;
	mMatrix[15] = 1.0f;

	// Выполним поворот
	glMultMatrixf(mMatrix);

	// Теперь отодвигаемся назад
	glTranslatef(-pCamera->vLocation[0], -pCamera->vLocation[1], -pCamera->vLocation[2]);
}

//**********************************************************
void CMFC_SDIView::gltApplyActorTransform(GLTFrame* pFrame)
{
	GLTMatrix mTransform;
	gltGetMatrixFromFrame(pFrame, mTransform);
	glMultMatrixf(mTransform);
}

//**********************************************************
void CMFC_SDIView::gltGetMatrixFromFrame(GLTFrame* pFrame, GLTMatrix mMatrix)
{
	GLTVector3 vXAxis;

	gltVectorCrossProduct(pFrame->vUp, pFrame->vForward, vXAxis);

	memcpy(mMatrix, vXAxis, sizeof(GLTVector3));
	mMatrix[3] = 0.0f;

	memcpy(mMatrix + 4, pFrame->vUp, sizeof(GLTVector3));
	mMatrix[7] = 0.0f;

	memcpy(mMatrix + 8, pFrame->vForward, sizeof(GLTVector3));
	mMatrix[11] = 0.0f;

	memcpy(mMatrix + 12, pFrame->vLocation, sizeof(GLTVector3));
	mMatrix[15] = 1.0f;
}


//**********************************************************
void CMFC_SDIView::gltNormalizeVector(GLTVector3 vNormal)
{
	GLfloat fLength = 1.0f / gltGetVectorLength(vNormal);
	gltScaleVector(vNormal, fLength);
}

//**********************************************************
void CMFC_SDIView::gltScaleVector(GLTVector3 vVector, const GLfloat fScale)
{
	vVector[0] *= fScale; vVector[1] *= fScale; vVector[2] *= fScale;
}


//**********************************************************
void CMFC_SDIView::gltInitFrame(GLTFrame* pFrame)
{
	pFrame->vLocation[0] = 0.0f;
	pFrame->vLocation[1] = 0.0f;
	pFrame->vLocation[2] = 0.0f;

	pFrame->vUp[0] = 0.0f;
	pFrame->vUp[1] = 1.0f;
	pFrame->vUp[2] = 0.0f;

	pFrame->vForward[0] = 0.0f;
	pFrame->vForward[1] = 0.0f;
	pFrame->vForward[2] = -1.0f;
}

//**********************************************************
void CMFC_SDIView::gltMoveFrameForward(GLTFrame* pFrame, GLfloat fStep)
{
	pFrame->vLocation[0] += pFrame->vForward[0] * fStep;
	pFrame->vLocation[1] += pFrame->vForward[1] * fStep;
	pFrame->vLocation[2] += pFrame->vForward[2] * fStep;
}

//**********************************************************
void CMFC_SDIView::gltRotateFrameLocalY(GLTFrame* pFrame, GLfloat fAngle)
{
	GLTMatrix mRotation;
	GLTVector3 vNewForward;

	gltRotationMatrix((float)gltDegToRad(fAngle), 0.0f, 1.0f, 0.0f, mRotation);
	gltRotationMatrix(fAngle, pFrame->vUp[0], pFrame->vUp[1], pFrame->vUp[2], mRotation);

	gltRotateVector(pFrame->vForward, mRotation, vNewForward);
	memcpy(pFrame->vForward, vNewForward, sizeof(GLTVector3));
}

//**********************************************************
// Создание матрицы поворота 4x4
void CMFC_SDIView::gltRotationMatrix(float angle, float x, float y, float z, GLTMatrix mMatrix)
{
	float vecLength, sinSave, cosSave, oneMinusCos;
	float xx, yy, zz, xy, yz, zx, xs, ys, zs;

	if (x == 0.0f && y == 0.0f && z == 0.0f)
	{
		gltLoadIdentityMatrix(mMatrix);
		return;
	}

	// Нормализуем вектор
	vecLength = (float)sqrt(x * x + y * y + z * z);
	x /= vecLength;
	y /= vecLength;
	z /= vecLength;

	sinSave = (float)sin(angle);
	cosSave = (float)cos(angle);
	oneMinusCos = 1.0f - cosSave;

	xx = x * x;
	yy = y * y;
	zz = z * z;
	xy = x * y;
	yz = y * z;
	zx = z * x;
	xs = x * sinSave;
	ys = y * sinSave;
	zs = z * sinSave;

	mMatrix[0] = (oneMinusCos * xx) + cosSave;
	mMatrix[4] = (oneMinusCos * xy) - zs;
	mMatrix[8] = (oneMinusCos * zx) + ys;
	mMatrix[12] = 0.0f;

	mMatrix[1] = (oneMinusCos * xy) + zs;
	mMatrix[5] = (oneMinusCos * yy) + cosSave;
	mMatrix[9] = (oneMinusCos * yz) - xs;
	mMatrix[13] = 0.0f;

	mMatrix[2] = (oneMinusCos * zx) - ys;
	mMatrix[6] = (oneMinusCos * yz) + xs;
	mMatrix[10] = (oneMinusCos * zz) + cosSave;
	mMatrix[14] = 0.0f;

	mMatrix[3] = 0.0f;
	mMatrix[7] = 0.0f;
	mMatrix[11] = 0.0f;
	mMatrix[15] = 1.0f;
}

//**********************************************************
void CMFC_SDIView::gltRotateVector(const GLTVector3 vSrcVector, const GLTMatrix mMatrix, GLTVector3 vOut)
{
	vOut[0] = mMatrix[0] * vSrcVector[0] + mMatrix[4] * vSrcVector[1] + mMatrix[8] * vSrcVector[2];
	vOut[1] = mMatrix[1] * vSrcVector[0] + mMatrix[5] * vSrcVector[1] + mMatrix[9] * vSrcVector[2];
	vOut[2] = mMatrix[2] * vSrcVector[0] + mMatrix[6] * vSrcVector[1] + mMatrix[10] * vSrcVector[2];
}

//**********************************************************
void CMFC_SDIView::gltVectorCrossProduct(const GLTVector3 vU, const GLTVector3 vV, GLTVector3 vResult)
{
	vResult[0] = vU[1] * vV[2] - vV[1] * vU[2];
	vResult[1] = -vU[0] * vV[2] + vV[0] * vU[2];
	vResult[2] = vU[0] * vV[1] - vV[0] * vU[1];
}

//**********************************************************
// Загрузка матрицы как единичной
void CMFC_SDIView::gltLoadIdentityMatrix(GLTMatrix m)
{
	static GLTMatrix identity = { 1.0f, 0.0f, 0.0f, 0.0f,
								 0.0f, 1.0f, 0.0f, 0.0f,
								 0.0f, 0.0f, 1.0f, 0.0f,
								 0.0f, 0.0f, 0.0f, 1.0f };

	memcpy(m, identity, sizeof(GLTMatrix));
}

//**********************************************************
// Устанавливается состояние инициализации
void CMFC_SDIView::SetupSphereWorld(void)
{
	gltInitFrame(&frameCamera);  // Инициализируем камеру

	// Генерируем центры 50 сфер случайным образом
	// в плоскости x0z внутри рамки от -20 до 20
	for (int iSphere = 0; iSphere < NUM_SPHERES; iSphere++)
	{
		gltInitFrame(&spheres[iSphere]);    // Инициализируем фрейм

		// На плоскости x0z размещаем случайно центры
		// сфер в диапазоне от -20 до 20 с шагом .1
		spheres[iSphere].vLocation[0] = (float)((rand() % 400) - 200) * 0.1f;
		spheres[iSphere].vLocation[1] = 0.0f;
		spheres[iSphere].vLocation[2] = (float)((rand() % 400) - 200) * 0.1f;
	}
}

void CMFC_SDIView::scena2() {
	

	RenderScene();  //Сцена-тор  сфера земля
	glFlush();
	SwapBuffers(wglGetCurrentDC());

}
