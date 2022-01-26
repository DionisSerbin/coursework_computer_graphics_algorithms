#define GL_SILENCE_DEPRECATION
#include <stdio.h>
#include <GLUT/GLUT.h>
#include <string.h>
#include <vector>

float left[1000];
float right[1000];
float z_buff[1000][1000];
void zbuffer();

void initialize(){
    glutInitWindowSize(1000,1000);
    glutCreateWindow("Z-buffering");
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,1000,0,1000);
    glFlush();
}

void render(int v){
    glClear(GL_COLOR_BUFFER_BIT);
    zbuffer();
}

void screen(){
    glutCreateMenu(render);
    glutAddMenuEntry("Z-Buffer",1);
    glutAttachMenu(GLUT_LEFT_BUTTON);
}

void depthPaint(float x,float y,float z){
    if (z > 0) {
        if(z < 5)
            glColor3f(0.6,0.0,0.0);
        if(z == 5)
            glColor3f(0.6,0.0,1.0);
        else if (z > 5)
            glColor3f(0.0,1.0,0.7);
    }
    else
        glColor3f(0.5,0.5,0.5);
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
    glFlush();
}

void drawFigure(float a[],float b[],float c[],float d[]){
    glColor3f(0.0,0.0,1.0);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(a[0],a[1]);
    glVertex2f(b[0],b[1]);
    glVertex2f(c[0],c[1]);
    glVertex2f(d[0],d[1]);
    glEnd();
}

void pushVector(std::vector<float> &vec, float mass[]){
    for (int i = 0; i < 3; i++) {
        vec.push_back(mass[i]);
    }
}

void calcOfNearZ(float firstTop[],float secondTop[]){
    float x,m;
    std::vector<float> near, far;// 1 - far
    int i;
    if(firstTop[1] > secondTop[1]){
        pushVector(near, secondTop);
        pushVector(far, firstTop);
    } else {
        pushVector(near, firstTop);
        pushVector(far, secondTop);
    }
    if(near[1] != far[1]){
        m = (far[0] - near[0]) / (far[1] - near[1]);
    } else {
        m = far[0] - near[0];
    }
    x = near[0];
    for (i = near[1]; i <= far[1]; i++) {
        if(x<left[i])
            left[i]=x;
        if(x>right[i])
            right[i]=x;
        x+=m;
    }
}

void zbuffer(){
    float fig1_1[3] = {400,400,7};
    float fig1_2[3] = {400,500,7};
    float fig1_3[3] = {500,500,7};
    float fig1_4[3] = {500,400,7};
    float fig2_1[3] = {500,400,4};
    float fig2_2[3] = {400,500,4};
    float fig2_3[3] = {500,600,4};
    float fig2_4[3] = {600,500,4};
    float fig3_1[3] = {400,500,5};
    float fig3_2[3] = {400,500,5};
    float fig3_3[3] = {600,550,5};
    float fig3_4[3] = {400,400,5};
    drawFigure(fig1_1,fig1_2,fig1_3,fig1_4);
    drawFigure(fig2_1,fig2_2,fig2_3,fig2_4);
    drawFigure(fig3_1,fig3_2,fig3_3,fig3_4);
    for(int i = 0; i < 1000; i++)
        for(int j = 0; j < 1000; j++)
            z_buff[i][j] = 0;
    for(int i = 0; i < 1000; i++){
        left[i] = 1000;
        right[i] = 0;
    }
    calcOfNearZ(fig1_1,fig1_2);
    calcOfNearZ(fig1_2,fig1_3);
    calcOfNearZ(fig1_3,fig1_4);
    calcOfNearZ(fig1_4,fig1_1);
    for(int i = 0; i < 1000; i++){
        if(left[i] <= right[i]){
            for(int j = left[i]; j <= right[i]; j++){
                if(z_buff[j][i] < fig1_1[2])
                    z_buff[j][i] = fig1_1[2];
            }
        }
    }
    for(int i = 0; i < 1000; i++){
        left[i] = 1000;
        right[i] = 0;
    }
    calcOfNearZ(fig2_1,fig2_2);
    calcOfNearZ(fig2_2,fig2_3);
    calcOfNearZ(fig2_3,fig2_4);
    calcOfNearZ(fig2_4,fig2_1);
    for(int i = 0; i < 1000; i++){
        if(left[i] <= right[i]){
            for(int j = left[i]; j <= right[i]; j++){
                if(z_buff[j][i] < fig2_1[2])
                    z_buff[j][i] = fig2_1[2];
            }
        }
    }
    for(int i = 0; i < 1000; i++){
        left[i] = 1000;
        right[i] = 0;
    }
    calcOfNearZ(fig3_1,fig3_2);
    calcOfNearZ(fig3_2,fig3_3);
    calcOfNearZ(fig3_3,fig3_4);
    calcOfNearZ(fig3_4,fig3_1);
    for(int i = 0; i < 1000; i++){
        if(left[i] <= right[i]){
            for(int j = left[i]; j <= right[i]; j++){
                if(z_buff[j][i] < fig3_1[2])
                    z_buff[j][i] = fig3_1[2];
            }
        }
    }
    
    for(int i= 395; i < 605; i++)
        for(int j = 395; j < 602; j++)
            depthPaint(i,j,z_buff[i][j]);
    glFlush();
}

void show(){
    screen();
}

int main(int argc, char * argv[]){
    glutInit(&argc,argv);
    initialize();
    glutDisplayFunc(show);
    glutMainLoop();
    return 0;
}
