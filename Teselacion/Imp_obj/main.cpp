#include <fstream>
#include <GL/glew.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>
#include <GL/freeglut.h>
#include <GL/GL.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>

#define _CRT_SECURE_NO_WARNINGS
#define WIDTH 1000
#define HEIGHT 800

float cameraDistance = 500.0f;
float objectPositionX = 0.0f;
float objectPositionY = 0.0f;
float objectRotationX = 0.0f;
float objectRotationY = 0.0f;

int prevMouseX = 0;
int prevMouseY = 0;
bool mousePressed = false;

float rotAng = 0.0f, spin = 0.0f;
int refreshRate = 5;
bool iskey = false, ismouse = false;
float posx = 0.0f;
void display();
void spinDisplayIzq();
void spinDisplayDer();
void keyUp(int key, int, int);
void keyDown(int key, int, int);
void mouse(int button, int state, int x, int y);



class Model {
private:
    class Face {
    public:
        int edge;
        int* vertices;
        int* texcoords;
        int normal;

        Face(int edge, int* vertices, int* texcoords, int normal = -1) {
            this->edge = edge;
            this->vertices = vertices;
            this->texcoords = texcoords;
            this->normal = normal;
        }
    };
    std::vector<float*> vertices;
    std::vector<float*> texcoords;
    std::vector<float*> normals;
    std::vector<Face> faces;
    GLuint list;

public:
    const std::vector<float*> getVertices() const {
        return vertices;
    }
    const std::vector<float*> getTexcoords() const {
        return texcoords;
    }
    const std::vector<float*> getNormals() const {
        return normals;
    }

    void load(const char* filename) {

        std::ifstream in(filename);
        if (!in.is_open()) {
            printf("Cannot load model %s\n", filename);
            return;
        }
        std::vector<std::string> lines;
        std::string line;
        while (!in.eof()) {
            std::getline(in, line);
            lines.push_back(line);
        }
        in.close();
        float a, b, c;
        for (std::string& line : lines) {
            if (line[0] == 'v') {
                if (line[1] == ' ') {
                    sscanf_s(line.c_str(), "v %f %f %f", &a, &b, &c);
                    vertices.push_back(new float[3] {a, b, c});
                }
                else if (line[1] == 't') {
                    sscanf_s(line.c_str(), "vt %f %f", &a, &b);
                    texcoords.push_back(new float[2] {a, b});
                }
                else {
                    sscanf_s(line.c_str(), "vn %f %f %f", &a, &b, &c);
                    normals.push_back(new float[3] {a, b, c});
                }
            }
            else if (line[0] == 'f') {
                int v0, v1, v2, t0, t1, t2, n;
                sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &t0, &n, &v1, &t1, &n, &v2, &t2, &n);
                int* v = new int[3] {v0 - 1, v1 - 1, v2 - 1};
                faces.push_back(Face(3, v, NULL, n - 1));
            }
        }
        list = glGenLists(1);
        glNewList(list, GL_COMPILE);
        for (Face& face : faces) {
            if (face.normal != -1)
                glNormal3fv(normals[face.normal]);
            else
                glDisable(GL_LIGHTING);
            glBegin(GL_TRIANGLES);
            for (int i = 1; i < face.edge - 1; i++) {
                glVertex3fv(vertices[face.vertices[0]]);
                glVertex3fv(vertices[face.vertices[i]]);
                glVertex3fv(vertices[face.vertices[i + 1]]);
            }
            glEnd();
            if (face.normal == -1)
                glEnable(GL_LIGHTING);
        }
        glEndList();
        printf("Model: %s\n", filename);
        printf("Vertices: %d\n", vertices.size());
        printf("Texcoords: %d\n", texcoords.size());
        printf("Normals: %d\n", normals.size());
        printf("Faces: %d\n", faces.size());
        for (float* f : vertices)
            delete f;
        vertices.clear();
        for (float* f : texcoords)
            delete f;
        texcoords.clear();
        for (float* f : normals)
            delete f;
        normals.clear();
        faces.clear();
    }
    void draw(int iterations) {
        for (int i = 0; i < iterations; i++) {
            glCallList(list);
        }
    }
};


Model model;
const std::vector<float*>& vertices = model.getVertices();
const std::vector<float*>& texcoords = model.getTexcoords();
const std::vector<float*>& normals = model.getNormals();

void init() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //Luz
    GLfloat light_pos[] = { -1.0f, 10.0f, 100.0f, 1.0f };
    //Habilita la iluminación
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)WIDTH / (float)HEIGHT, 1.0, 2000.0); // Modificar el ángulo de visión a 45 grados
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 500.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Ajustar la posición de la cámara
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

   model.load("../Imp_obj/Lowpoly_Fox.obj");
    //model.load("../Imp_obj/pard.obj"); modelo que también funciona
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Configuración de la cámara y rotación del objeto
    glTranslatef(objectPositionX + posx, objectPositionY, -cameraDistance);
    glRotatef(objectRotationY, 1.0f, 0.0f, 0.0f);
    glRotatef(objectRotationX, 0.0f, 1.0f, 0.0f);

    // Renderizar el primer objeto con la teselación deseada
    model.draw(1);

    // Configurar la proyección de la sombra
    GLfloat light_pos[] = { -1.0f, 10.0f, 100.0f, 1.0f };
    GLdouble ground_plane[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glPushMatrix();
    glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
    glClipPlane(GL_CLIP_PLANE0, ground_plane);
    glEnable(GL_CLIP_PLANE0);
    glTranslatef(0.05f, -10.01f, -130.0f);
    glScalef(1.0f, 0.0f, 1.0f);

    // Renderizar el primer objeto nuevamente para la sombra
    model.draw(1);

    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Cambiar el modo de representación de polígonos a líneas y configurar el sombreado
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glShadeModel(GL_FLAT);

    // Renderizar el primer objeto con una menor teselación
    model.draw(1);

    glutSwapBuffers();
}



void spinDisplayIzq() {
    if (ismouse)
    {
        spin -= 0.1;
        if (spin > 360)
        {
            spin -= 360;
        }
    }
    if (iskey)
        posx -= 0.005;
    glutPostRedisplay();
}

void spinDisplayDer() {
    if (ismouse)
    {
        spin += 0.1;
        if (spin > 360)
        {
            spin -= 360;
        }
    }
    if (iskey)
        posx += 0.005;
    glutPostRedisplay();
}
void mouse(int button, int state, int x, int y) {
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            ismouse = true;
            glutIdleFunc(spinDisplayIzq);

        }
        else if (state == GLUT_UP)
        {
            ismouse = false;
            glutIdleFunc(NULL);
        }
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN) {
            ismouse = true;
            glutIdleFunc(spinDisplayDer);

        }
        else if (state == GLUT_UP)
        {
            ismouse = false;
            glutIdleFunc(NULL);
        }
        break;
    default:
        break;
    }
}

void keyDown(int key, int, int) {
    switch (key) {
    case GLUT_KEY_LEFT:
        iskey = true;
        glutIdleFunc(spinDisplayIzq);
        break;
    case GLUT_KEY_RIGHT:
        iskey = true;
        glutIdleFunc(spinDisplayDer);
        break;
    default:
        break;
    }
}

void keyUp(int key, int, int) {
    switch (key) {
    case GLUT_KEY_LEFT:
        iskey = false;
        glutIdleFunc(NULL);
        break;
    case GLUT_KEY_RIGHT:
        iskey = false;
        glutIdleFunc(NULL);
        break;
    default:
        break;
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    glutSetOption(GLUT_MULTISAMPLE, 8);

    int POS_X = (glutGet(GLUT_SCREEN_WIDTH) - WIDTH) >> 1;
    int POS_Y = (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) >> 1;
    glutInitWindowPosition(POS_X, POS_Y);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("OBJ con luces y sombra");
    init();
    glutSpecialFunc(keyDown);
	glutSpecialUpFunc(keyUp);
	glutMouseFunc(mouse);
    glutDisplayFunc(display); 


    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}