#define _CRT_SECURE_NO_WARNINGS
//opencv
#include "cv.h"
#include "highgui.h"
//opengl
#include <GL\glut.h>
//openmesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <iostream>

typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;

/*读取到的模型*/
MyMesh mesh;

/*窗口宽高*/
int winWidth = 500;
int winHeight = 600;

/*屏幕坐标与opengl坐标的转换比率*/
float ratio = 0.001f;
/*视口显示范围*/
float range = 1.0f;

/*旋转角度*/
float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
/*平移距离*/
float distanceX = 0.0f;
float distanceY = 0.0f;
float distanceZ = 0.0f;
/*缩放大小*/
float scaleX = 1.0f;
float scaleY = 1.0f;
float scaleZ = 1.0f;

/*鼠标的之前坐标*/
float lastPosX = 0.0f;
float lastPosY = 0.0f;
/*鼠标的左右键是否被按下*/
bool mouseLeft = false;
bool mouseRight = false;

/*图像数据*/
GLint     imageWidth;
GLint     imageHeight;
GLubyte*  pixelData;
GLint     pixelLength;
/*保存图像的纹理*/
GLuint textureID;

/*语义属性身高、体重*/
int attrHeight;
int attrWeight;

/*读取图像*/
bool readBmp(const char* image) {

	FILE* img = fopen(image, "rb");
	if (img == 0) {
		return false;
	}

	fseek(img, 0x0012, SEEK_SET);
	fread(&imageWidth, sizeof(imageWidth), 1, img);
	fread(&imageHeight, sizeof(imageHeight), 1, img);

	pixelLength = imageWidth * 3;
	while (pixelLength % 4 != 0) {
		++pixelLength;
	}
	pixelLength *= imageHeight;

	pixelData = (GLubyte*)malloc(pixelLength);
	if (pixelData == 0) {
		return false;
	}

	fseek(img, 54, SEEK_SET);
	fread(pixelData, pixelLength, 1, img);

	fclose(img);
	return true;
}

/*图像保存至纹理*/
bool genTexture() {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixelData);
	return textureID != -1;
}

/*读取模型*/
bool readMesh(char* meshFile) {
	mesh.request_vertex_normals();
	mesh.request_face_normals();
	if (!OpenMesh::IO::read_mesh(mesh, meshFile)) {
		return false;
	}
	mesh.update_normals();
	//mesh.release_face_normals();
	//mesh.release_vertex_normals();
	return true;
}

/*灯光设置*/
void setupLights() {
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPos[] = { 50.0f, 80.0f, 60.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight);
	glMateriali(GL_FRONT, GL_SHININESS, 100);
}

/*初始化显示设置以及读取图像、模型*/
void initSettings(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	setupLights();

	if (!readBmp("datasource/sumo.bmp")) {
		std::cerr << "Error: Cannot read image datasource/sumo.bmp." << std::endl;
		exit(0);
	}
	if (!genTexture()) {
		std::cerr << "Error: Cannot generate texture." << std::endl;
		exit(0);
	}
	if (!readMesh("datasource/s1p0.obj")) {
		std::cerr << "Error: Cannot read mesh datasource/s1p0.obj." << std::endl;
		exit(0);
	}

}

/*显示模型*/
void showFlat() {
	mesh.request_face_normals();
	mesh.update_normals();
	glColor3f(0.83f, 0.83f, 0.83f);
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
		glBegin(GL_TRIANGLES);
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
			glNormal3fv(mesh.normal(*fv_it).data());
			glVertex3fv(mesh.point(*fv_it).data());
		}
		glEnd();
	}
	mesh.release_face_normals();
}

/*绘制坐标轴*/
void drawAxes() {
	//绘制X轴
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.8f, 0.0f, 0.0f);
	glEnd();
	glPushMatrix();
		glTranslatef(0.8f, 0.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glutSolidCone(0.02, 0.08, 4, 4);
	glPopMatrix();

	//绘制Y轴
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.8f, 0.0f);
	glEnd();
	glPushMatrix();
		glTranslatef(0.0f, 0.8f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glutSolidCone(0.02, 0.08, 4, 4);
	glPopMatrix();

	//绘制Z轴
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.8f);
	glEnd();
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.8f);
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		glutSolidCone(0.02, 0.08, 4, 4);
	glPopMatrix();
}

/*显示绘制*/
void displayFunc(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//绘制图像
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.55f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.55f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.55f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.55f, 1.0f, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//绘制模型
	glPushMatrix();
		glScalef(scaleX, scaleY, scaleZ);
		glTranslatef(distanceX, distanceY, distanceZ);
		glRotatef(angleX, 1.0f, 0.0f, 0.0f);
		glRotatef(angleY, 0.0f, 1.0f, 0.0f);
		glRotatef(angleZ, 0.0f, 0.0f, 1.0f);
		drawAxes();
		showFlat();
	glPopMatrix();

	glutSwapBuffers();
}

/*窗口尺寸改变*/
void reshapeFunc(GLint width, GLint height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	if (width <= height) {
		glOrtho(-range, range, -range / aspect, range / aspect, -range, range);
	} else {
		glOrtho(-range * aspect, range * aspect, -range, range, -range, range);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*鼠标移动处理*/
void motionFunc(int x, int y) {
	if (mouseRight) {
		angleX += y - lastPosY;
		angleZ += x - lastPosX;
	} else if (mouseLeft) {
		distanceX += (x - lastPosX) * ratio;
		distanceY += (lastPosY - y) * ratio;
	}
	lastPosX = x;
	lastPosY = y;
	glutPostRedisplay();
}

/*鼠标按键处理*/
void mouseFunc(int button, int state, int x, int y) {
	lastPosX = x;
	lastPosY = y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouseLeft = true;
		mouseRight = false;
	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		mouseRight = true;
		mouseLeft = false;
	}
}

/*键盘按键处理*/
void keyboardFunc(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		scaleX += 0.05f;
		break;
	case '2':
		scaleY += 0.05f;
		break;
	case '3':
		scaleZ += 0.05f;
		break;
	case '4':
		scaleX -= 0.05f;
		break;
	case '5':
		scaleY -= 0.05f;
		break;
	case '6':
		scaleZ -= 0.05f;
		break;
	default:
		scaleX = 1.0f;
		scaleY = 1.0f;
		scaleZ = 1.0f;
		break;
	}
	glutPostRedisplay();
}

/*调节身高属性*/
void heightSlide(int pos) {
	attrHeight = pos;
}

/*调节体重属性*/
void weightSlide(int pos) {
	attrWeight = pos;
}

int main(int argc, char** argv) {
	//键盘按键选项
	std::cout << "Options:" << std::endl;
	std::cout << "1 - zoon in X" << std::endl;
	std::cout << "2 - zoon in Y" << std::endl;
	std::cout << "3 - zoon in Z" << std::endl;
	std::cout << "4 - zoom out X" << std::endl;
	std::cout << "5 - zoom out Y" << std::endl;
	std::cout << "6 - zoom out Z" << std::endl;
	std::cout << "left button of mouse move - translate" << std::endl;
	std::cout << "right button of mouse move - rotate" << std::endl;

	//属性调节窗口
	cvNamedWindow("Attributes", CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("Height", "Attributes", &attrHeight, 100, heightSlide);
	cvCreateTrackbar("Weight", "Attributes", &attrWeight, 100, weightSlide);

	//模型窗口
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Project2");

	initSettings();

	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(displayFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMainLoop();

	if (pixelData) {
		free(pixelData);
		pixelData = NULL;
	}

	cvWaitKey(0);
	cvDestroyAllWindows();

	return 0;
}
