#define GL_SILENCE_DEPRECATION
#include <GLUT/GLUT.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#define shadowBuffer 512
#define width 600
#define height 500
float centerX = 0.0f;
float centerZ = 0.0f;
float centerY = 0.0;
float eyeX = 0.0f;
float eyeZ = -2.5f;
float eyeY = 0.8;
float upX = 0.0f;
float upY = 1.0f;
float upZ = 0.0f;
GLuint shadowTexture;
float lightCenterX = 2.0f;
float lightCenterY = 2.5f;
float lightCenterZ = -2.0f;
float lightEyeX = 0.0f;
float lightEyeY = 0.0f;
float lightEyeZ = 0.0f;
float lightUpX = 0.0f;
float lightUpY = 1.0f;
float lightUpZ = 0.0f;
glm::mat4 lightMatrixProj;
glm::mat4 lightMatrixView;
glm::mat4 camMatrixProj;
glm::mat4 camMatrixView;

void setTextureParamsNear(GLenum mode, GLenum pnam){
    glTexParameteri(mode, pnam, GL_NEAREST);
}

void setTextureParamsClamp(GLenum mode, GLenum pnam){
    glTexParameteri(mode, pnam, GL_CLAMP);
}

void genShadMapTexture(GLenum mode){
    glGenTextures(1, &shadowTexture);
    glBindTexture(mode, shadowTexture);
    glTexImage2D(mode, 0, GL_DEPTH_COMPONENT, shadowBuffer, shadowBuffer, 0,
                    GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    setTextureParamsNear(mode, GL_TEXTURE_MIN_FILTER);
    setTextureParamsNear(mode, GL_TEXTURE_MAG_FILTER);
    setTextureParamsClamp(mode, GL_TEXTURE_WRAP_S);
    setTextureParamsClamp(mode, GL_TEXTURE_WRAP_T);
    glEnable(GL_COLOR_MATERIAL);
}

void makeCameraView(){
    glLoadIdentity();
    float aspect = (float) width / height;
    gluPerspective(45.0f, aspect, 1.0f, 100.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, camMatrixProj);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,
                centerX, centerY,  centerZ,
                upX, upY,  upZ);
    glGetFloatv(GL_MODELVIEW_MATRIX, camMatrixView);
}

void makeLightView(){
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 2.0f, 8.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightMatrixProj);
    glLoadIdentity();
    gluLookAt(lightCenterX, lightCenterY, lightCenterZ,
                lightEyeX, lightEyeY, lightEyeZ,
                lightUpX, lightUpY, lightUpZ);
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
    glutInitDisplayMode(GLUT_DOUBLE
                        | GLUT_RGB |
                        GLUT_DEPTH);
    glutInitWindowSize(600, 500);
    glutCreateWindow("Shadow Mapping");
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    makeDepthTest();
    genShadMapTexture(GL_TEXTURE_2D);
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

void loadmatr(GLenum mode, GLfloat *m){
    glMatrixMode(mode);
    glLoadMatrixf(m);
}

void makeLightPoint(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    loadmatr(GL_PROJECTION, lightMatrixProj);
    loadmatr(GL_MODELVIEW, lightMatrixView);
}

void setLightfv(GLenum pname, GLfloat *p){
    glLightfv(GL_LIGHT1, pname, p);
}

void drawFromCamer(){
    glClear(GL_DEPTH_BUFFER_BIT);
    loadmatr(GL_PROJECTION, camMatrixProj);
    loadmatr(GL_MODELVIEW, camMatrixView);
    glViewport(0, 0, width, height);
    setLightfv(GL_POSITION, glm::vec4(glm::vec3(lightCenterX, lightCenterY, lightCenterZ)));
    setLightfv(GL_DIFFUSE, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * 0.5f);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    drawFigure();
}

glm::mat4 projMatrix(float projValue){
    glm::mat4 projMatrix(projValue, 0.0f, 0.0f, 0.0f,
                       0.0f, projValue, 0.0f, 0.0f,
                       0.0f, 0.0f, projValue, 0.0f,
                    projValue, projValue, projValue, projValue * 2);;
    return projMatrix * lightMatrixProj * lightMatrixView;
}

void makeTextureCoord(GLenum coord, mat4 shadowMatrix,
                      int n, GLenum cap){
    glTexGeni(coord, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(coord, GL_EYE_PLANE, shadowMatrix.GetRow(n));
    glEnable(cap);
}

void setTextureParams(GLenum mode, GLenum pnam, GLint param){
    glTexParameteri(mode, pnam, param);
}

void buildShadows(GLenum mode){
    setLightfv(GL_DIFFUSE, vec4(1.0f, 1.0f, 1.0f, 1.0f));
    setLightfv(GL_SPECULAR, vec4(1.0f, 1.0f, 1.0f, 1.0f));
    glm::mat4 shadowMatrix = projMatrix(0.5f);
    makeTextureCoord(GL_S, shadowMatrix, 0, GL_TEXTURE_GEN_S);
    makeTextureCoord(GL_T, shadowMatrix, 1, GL_TEXTURE_GEN_T);
    makeTextureCoord(GL_R, shadowMatrix, 2, GL_TEXTURE_GEN_R);
    makeTextureCoord(GL_Q, shadowMatrix, 3, GL_TEXTURE_GEN_Q);
    glBindTexture(mode, shadowTexture);
    glEnable(mode);
    setTextureParams(mode, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
    setTextureParams(mode, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
    setTextureParams(mode, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
    glAlphaFunc(GL_GEQUAL, 0.99f);
    glEnable(GL_ALPHA_TEST);
    drawFigure();
}

void setShadeModel(GLenum mode, GLenum type){
    glCullFace(mode);
    glShadeModel(type);
}

void makeDisable(GLenum cap){
    glDisable(cap);
}

void render(){
    GLenum mode = GL_TEXTURE_2D;
    makeLightPoint();
    glViewport(0, 0, shadowBuffer, shadowBuffer);
    setShadeModel(GL_FRONT, GL_FLAT);
    glColorMask(0, 0, 0, 0);
    drawFigure();
    glBindTexture(mode, shadowTexture);
    glCopyTexSubImage2D(mode, 0, 0, 0, 0, 0,
                        shadowBuffer, shadowBuffer);
    setShadeModel(GL_BACK, GL_SMOOTH);
    glColorMask(1, 1, 1, 1);
    drawFromCamer();
    buildShadows(mode);
    makeDisable(GL_TEXTURE_GEN_S);
    makeDisable(GL_TEXTURE_GEN_T);
    makeDisable(GL_TEXTURE_GEN_R);
    makeDisable(GL_TEXTURE_GEN_Q);
    makeDisable(GL_LIGHTING);
    makeDisable(GL_ALPHA_TEST);
    glFinish();
    glutSwapBuffers();
    glutPostRedisplay();
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
    gluLookAt(eyeX, eyeY, eyeZ,
              centerX, centerY,  centerZ,
              upX, upY,  upZ);
    glGetFloatv(GL_MODELVIEW_MATRIX, camMatrixView);
}

void handleAction(){
    glutDisplayFunc(render);
    glutKeyboardFunc(keypress);
    glutMainLoop();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    initialize();
    handleAction();
    return 0;
}
