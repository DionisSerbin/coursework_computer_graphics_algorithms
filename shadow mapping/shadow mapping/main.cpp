#define GL_SILENCE_DEPRECATION
#include <GLUT/GLUT.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.h>
int width, height;
float centerX = 0.0f;
float centerZ = 0.0f;
float centerY = 0.0;
float eyeX = 0.0f;
float eyeZ = -1.5f;
float eyeY = 0.8;
float upX = 0.0f;
float upY = 1.0f;
float upZ = 0.0f;
const int shadowBuffer = 512;
GLuint shadowTexture;
vec3 light(2.0f, 2.6f,-2.0f);
mat4x4 lightMatrixProj;
mat4x4 lightMatrixView;
mat4x4 camMatrixProj;
mat4x4 camMatrixView;

void genShadMapTexture(){
    glGenTextures(1, &shadowTexture);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowBuffer, shadowBuffer, 0,
                    GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glEnable(GL_COLOR_MATERIAL);
}

void makeCameraView(){
    glLoadIdentity();
    gluPerspective(45.0f, (float)width/height, 1.0f, 100.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, camMatrixProj);

    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,
                centerX, centerY,  centerZ,
                0.0f, 1.0f,  0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, camMatrixView);
}

void makeLightView(){
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 2.0f, 8.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightMatrixProj);

    glLoadIdentity();
    gluLookAt(light.x, light.y, light.z,
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightMatrixView);
}

void makeDepthTest(){
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
}

bool initialize(){
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 500);
    glutCreateWindow("Shadow Mapping");
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    makeDepthTest();
    genShadMapTexture();
    glPushMatrix();
    makeCameraView();
    makeLightView();
    glPopMatrix();
    return true;
}

void drawFigure(){
    // платформа
    glPushMatrix();
    glColor3f(0.8f, 0.7f, 0.8f);
    glTranslatef(0.0f, 0.0f, 1.0f);
    glScalef(0.95f, 0.06f, 0.95f);
    glutSolidCube(3.0f);
    glPopMatrix();
    glPushMatrix();
    
    // тор
    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    glutSolidTorus(0.05f, 0.2f, 50, 50);
    glPopMatrix();

    // сфера
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.2f, 0.4f, 0.4f);
    glutSolidSphere(0.2f, 20, 20);
    glPopMatrix();
    
    // куб
    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.5f, 0.4f, 0.5f);
    glutSolidCube(0.3f);
    glPopMatrix();
    
    // восьмигранник
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.2f, 0.4f, 0.9f);
    glScalef(0.2f, 0.2f, 0.2f);
    glutSolidOctahedron();
    glPopMatrix();
}

void makeLightPoint(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(lightMatrixProj);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(lightMatrixView);
}

void drawFromCamer(){
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(camMatrixProj);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(camMatrixView);
    glViewport(0, 0, width, height);
    glLightfv(GL_LIGHT1, GL_POSITION, vec4(light));
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white * 0.2f);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    drawFigure();
}

void buildShadows(){
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    static mat4x4 depthMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                                0.0f, 0.5f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.5f, 0.0f,
                                0.5f, 0.5f, 0.5f, 1.0f);
    mat4x4 shadowMatrix = depthMatrix * lightMatrixProj * lightMatrixView;
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_S, GL_EYE_PLANE, shadowMatrix.GetRow(0));
    glEnable(GL_TEXTURE_GEN_S);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_T, GL_EYE_PLANE, shadowMatrix.GetRow(1));
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_R, GL_EYE_PLANE, shadowMatrix.GetRow(2));
    glEnable(GL_TEXTURE_GEN_R);
    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_Q, GL_EYE_PLANE, shadowMatrix.GetRow(3));
    glEnable(GL_TEXTURE_GEN_Q);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
    glAlphaFunc(GL_GEQUAL, 0.99f);
    glEnable(GL_ALPHA_TEST);
    drawFigure();
}

void render(){
    makeLightPoint();
    glViewport(0, 0, shadowBuffer, shadowBuffer);
    glCullFace(GL_FRONT);
    glShadeModel(GL_FLAT);
    glColorMask(0, 0, 0, 0);
    drawFigure();
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                        shadowBuffer, shadowBuffer);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glColorMask(1, 1, 1, 1);
    drawFromCamer();
    buildShadows();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glDisable(GL_LIGHTING);
    glDisable(GL_ALPHA_TEST);
    glFinish();
    glutSwapBuffers();
    glutPostRedisplay();
}


void resize(int w, int h) {
    width = w;
    height = h;
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(45.0f, (float) width / height, 1.0f, 100.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, camMatrixProj);
    glPopMatrix();
}

void keypress(unsigned char key, int xx, int yy) {
    if(key==27)// esc
        exit(0);
    if (key == 113){// q
        eyeX += 0.1f;
    }
    if (key == 119){// w
        eyeY += 0.1f;
    }
    if (key == 101){ //e
        eyeZ += 0.1f;
    }
    if (key == 97){ // a
        centerX += 0.1f;
    }
    if (key == 115) { //s
        centerY += 0.1f;
    }
    if(key == 100){ //d
        centerZ += 0.1f;
    }
    if(key == 122){ //z
        upX += 0.1f;
    }
    if(key == 120){//x
        upY += 0.1f;
    }
    if(key == 99) { //c
        upZ += 0.1f;
    }
    if (key == 114){// r
        eyeX -= 0.1f;
    }
    if (key == 116){// t
        eyeY -= 0.1f;
    }
    if (key == 121){ //y
        eyeZ -= 0.1f;
    }
    if (key == 102){ // f
        centerX -= 0.1f;
    }
    if (key == 103) { //g
        centerY -= 0.1f;
    }
    if(key == 104){ //h
        centerZ -= 0.1f;
    }
    if(key == 118){ //v
        upX -= 0.1f;
    }
    if(key == 98){//b
        upY -= 0.1f;
    }
    if(key == 110) { //n
        upZ -= 0.1f;
    }
    glLoadIdentity();
    gluLookAt(    eyeX, eyeY, eyeZ,
                centerX, centerY,  centerZ,
                upX, upY,  upZ);
    glGetFloatv(GL_MODELVIEW_MATRIX, camMatrixView);
}

void handleAction(){
    glutDisplayFunc(render);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keypress);
    glutMainLoop();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    initialize();
    handleAction();
    return 0;
}
