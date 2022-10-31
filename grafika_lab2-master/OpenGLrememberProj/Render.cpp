#include "Render.h"

#include <sstream>
#include <iostream>
#include <list>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
using namespace std;


#define HEIGHT 4
#define pdd pair<double, double>


void figure2();
void figure(int i = 0);
void round(double step, double pointA[], double pointB[], double pointC[]);
void roundIn(double step, double pointA[], double pointB[], double pointC[], double pointA1[], double E[], double H[]);
void lineFromPoints(pdd P, pdd Q, double& a, double& b, double& c);
void perpendicularBisectorFromLine(pdd P, pdd Q, double& a, double& b, double& c);
pdd lineLineIntersection(double a1, double b1, double c1, double a2, double b2, double c2);
pdd findCircumCenter(pdd P, pdd Q, pdd R);
void twist();
list<double> vectProz(double A[], double B[], double C[], int k);

bool textureMode = true;
bool lightMode = true;

int switcher = 0;

//Кординаты текстур верхней крышки
double A1_texture[] = {0.304, 0.897};
double B1_texture[] = { 0.266, 0.991 };
double C1_texture[] = { 0.133, 0.920 };
double D1_texture[] = { 0.039, 0.972 };
double E1_texture[] = { 0.097, 0.897 };
double F1_texture[] = { 0, 0.805 };
double G1_texture[] = { 0.153, 0.714 };
double H1_texture[] = { 0.172, 0.860 };
double pointC1_texture[] = {0.096, 0.805};

//Кординаты текстур нижней крышки
double A_texture[] = { 0.745, 0.564 };
double B_texture[] = { 0.689, 0.649 };
double C_texture[] = { 0.825, 0.714 };
double D_texture[] = { 0.833, 0.816 };
double E_texture[] = { 0.862, 0.732 };
double F_texture[] = { 0.991, 0.756 };
double G_texture[] = { 0.981, 0.583 };
double H_texture[] = { 0.851, 0.649 };
double pointC_texture[] = { 0.940, 0.691 };


//Боковые стороны
double Ab_texture[] = { 0.304, 0.822 };
double Bb_texture[] = { 0.467, 0.822 };
double Cb_texture[] = { 0.620, 0.822 };
double Db_texture[] = { 0.734, 0.822 };
double Eb_texture[] = { 0.833, 0.822 };
double Fb_texture[] = { 0.969, 0.822 };


double Gb_texture[] = { 0.368, 0.640 };
double Hb_texture[] = { 0.522, 0.640 };

double A1b_texture[] = { 0.304, 0.897 };
double B1b_texture[] = { 0.467, 0.897 };
double C1b_texture[] = { 0.620, 0.897 };
double D1b_texture[] = { 0.734, 0.897 };
double E1b_texture[] = { 0.833, 0.897 };
double F1b_texture[] = { 0.969, 0.897 };

double G1b_texture[] = { 0.368, 0.712 };
double H1b_texture[] = { 0.522, 0.712 };


double F1add_texture[] = { 0.153, 0.712 };
double Fadd_texture[] = { 0.153, 0.640 };

double A1add_texture[] = { 0.662, 0.712 };
double Aadd_texture[] = { 0.662, 0.640 };

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;


	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}


	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света


GLuint texId;

//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	

	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'K')
	{
		switcher++;
		if (switcher > 2) switcher = 0;
		//настройка текстур

	//4 байта на хранение пикселя
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		//настройка режима наложения текстур
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		//включаем текстуры
		glEnable(GL_TEXTURE_2D);

		//glEnable(GL_TEXTURE_3D);


		//массив трехбайтных элементов  (R G B)
		RGBTRIPLE* texarray = NULL;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)

		char* texCharArray = NULL;
		int texW, texH;
		switch (switcher)
		{
		case 0:OpenGL::LoadBMP("textures/tx1.bmp", &texW, &texH, &texarray);
			break;

		case 1:OpenGL::LoadBMP("textures/tx2.bmp", &texW, &texH, &texarray);
			break;
		case 2:OpenGL::LoadBMP("textures/tx3.bmp", &texW, &texH, &texarray);
			break;
		}


		OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



		//генерируем ИД для текстуры
		glGenTextures(1, &texId);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, texId);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

		//отчистка памяти
		free(texCharArray);
		free(texarray);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	}

	if (key == 'J')
	{

		switcher--;
		if (switcher < 0) switcher = 2;
		//настройка текстур

	//4 байта на хранение пикселя
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		//настройка режима наложения текстур
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		//включаем текстуры
		glEnable(GL_TEXTURE_2D);

		//glEnable(GL_TEXTURE_3D);


		//массив трехбайтных элементов  (R G B)
		RGBTRIPLE* texarray = NULL;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)

		char* texCharArray = NULL;
		int texW, texH;
		switch (switcher)
		{
		case 0:OpenGL::LoadBMP("textures/tx1.bmp", &texW, &texH, &texarray);
			break;

		case 1:OpenGL::LoadBMP("textures/tx2.bmp", &texW, &texH, &texarray);
			break;
		case 2:OpenGL::LoadBMP("textures/tx3.bmp", &texW, &texH, &texarray);
			break;
		}
		

		OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



		//генерируем ИД для текстуры
		glGenTextures(1, &texId);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, texId);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

		//отчистка памяти
		free(texCharArray);
		free(texarray);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}





//выполняется перед первым рендером
void initRender(OpenGL* ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);

	//glEnable(GL_TEXTURE_3D);


	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE* texarray = NULL;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	
	char* texCharArray = NULL;
	int texW, texH;
	
	OpenGL::LoadBMP("textures/tx1.bmp", &texW, &texH, &texarray);
	
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);
	


	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH);


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  
	figure(1);
	
	//figure2();

	//Сообщение вверху экрана


	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
									//(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}


void figure2()
{
	double A[3] = { -2, -2, 10 };
	double B[3] = { -2, 2, 10 };
	double C[3] = { 2,2, 10 };
	double D[3] = { 2,-2, 10 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 1);
	glVertex3dv(B);
	glTexCoord2d(1, 1);
	glVertex3dv(C);
	glTexCoord2d(1, 0);
	glVertex3dv(D);

	glEnd();
}
void figure(int i)
{
	list <double> N;

	double A[] = { 9, 1, 0 };
	double B[] = { 7, 6, 0 };
	double C[] = { 0, 2, 0 };
	double D[] = { -5, 5, 0 };
	double E[] = { -2, 1, 0 };
	double F[] = { -7, -4, 0 };
	double G[] = { 1,-9, 0 };
	double H[] = { 2,-1, 0 };

	double A1[] = { 9, 1, HEIGHT };
	double B1[] = { 7, 6, HEIGHT };
	double C1[] = { 0, 2, HEIGHT };
	double D1[] = { -5, 5, HEIGHT };
	double E1[] = { -2, 1, HEIGHT };
	double F1[] = { -7, -4, HEIGHT };
	double G1[] = { 1,-9, HEIGHT };
	double H1[] = { 2,-1, HEIGHT };


	glBindTexture(GL_TEXTURE_2D, texId);

	glBegin(GL_TRIANGLES);

	glColor3d(0.25, 0.15, 0.1);

	N = vectProz(A, B, H, -1);
	auto iter = N.begin();
	double a, b, c;
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(A_texture[0], A_texture[1]);
	glVertex3dv(A);
	glTexCoord2d(B_texture[0], B_texture[1]);
	glVertex3dv(B);
	glTexCoord2d(H_texture[0], H_texture[1]);
	glVertex3dv(H);



	N = vectProz(H, B, C, -1);
	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(H_texture[0], H_texture[1]);
	glVertex3dv(H);
	glTexCoord2d(B_texture[0], B_texture[1]);
	glVertex3dv(B);
	glTexCoord2d(C_texture[0], C_texture[1]);
	glVertex3dv(C);



	N = vectProz(H, C, E, -1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(H_texture[0], H_texture[1]);
	glVertex3dv(H);
	glTexCoord2d(C_texture[0], C_texture[1]);
	glVertex3dv(C);
	glTexCoord2d(E_texture[0], E_texture[1]);
	glVertex3dv(E);

	N = vectProz(C, D, E, -1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(C_texture[0], C_texture[1]);
	glVertex3dv(C);
	glTexCoord2d(D_texture[0], D_texture[1]);
	glVertex3dv(D);
	glTexCoord2d(E_texture[0], E_texture[1]);
	glVertex3dv(E);



	//--------


	glEnd();





	


	glBegin(GL_QUADS);

	//---------------
	glColor3d(0.25, 0.15, 0.1);



	N = vectProz(A, A1, H1, 1);

	iter = N.begin();

	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(Hb_texture[0], Hb_texture[1]);
	glVertex3dv(H);
	glTexCoord2d(H1b_texture[0], H1b_texture[1]);
	glVertex3dv(H1);
	glTexCoord2d(A1add_texture[0], A1add_texture[1]);
	glVertex3dv(A1);
	glTexCoord2d(Aadd_texture[0], Aadd_texture[1]);
	glVertex3dv(A);



	N = vectProz(A, A1, B1, -1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	/*glColor3d(dis(gen) + 0.3, dis(gen) + 0.2, dis(gen) + 0.2);
	glVertex3dv(A);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(B);*/


	//glColor3d(dis(gen) + 0.2, dis(gen) + 0.3, dis(gen) + 0.2);

	N = vectProz(B, B1, C1, -1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(Bb_texture[0], Bb_texture[1]);
	glVertex3dv(B);
	glTexCoord2d(B1b_texture[0], B1b_texture[1]);
	glVertex3dv(B1);
	glTexCoord2d(C1b_texture[0], C1b_texture[1]);
	glVertex3dv(C1);
	glTexCoord2d(Cb_texture[0], Cb_texture[1]);
	glVertex3dv(C);


	//glColor3d(dis(gen) + 0.2, dis(gen) + 0.2, dis(gen) + 0.3);
	N = vectProz(C, C1, D1, -1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(Cb_texture[0], Cb_texture[1]);
	glVertex3dv(C);
	glTexCoord2d(C1b_texture[0], C1b_texture[1]);
	glVertex3dv(C1);
	glTexCoord2d(D1b_texture[0], D1b_texture[1]);
	glVertex3dv(D1);
	glTexCoord2d(Db_texture[0], Db_texture[1]);
	glVertex3dv(D);


	//	glColor3d(dis(gen) + 0.2, dis(gen) + 0.4, dis(gen) + 0.2);
	N = vectProz(D, D1, E1, -1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(Db_texture[0], Db_texture[1]);
	glVertex3dv(D);
	glTexCoord2d(D1b_texture[0], D1b_texture[1]);
	glVertex3dv(D1);
	glTexCoord2d(E1b_texture[0], E1b_texture[1]);
	glVertex3dv(E1);
	glTexCoord2d(Eb_texture[0], Eb_texture[1]);
	glVertex3dv(E);


	//	glColor3d(dis(gen) + 0.2, dis(gen) + 0.2, dis(gen) + 0.4);
	N = vectProz(E, E1, F1, -1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(Eb_texture[0], Eb_texture[1]);
	glVertex3dv(E);
	glTexCoord2d(E1b_texture[0], E1b_texture[1]);
	glVertex3dv(E1);
	glTexCoord2d(F1b_texture[0], F1b_texture[1]);
	glVertex3dv(F1);
	glTexCoord2d(Fb_texture[0], Fb_texture[1]);
	glVertex3dv(F);

	N = vectProz(F, F1, G1, -1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	/*glColor3d(dis(gen) + 0.5, dis(gen) + 0.2, dis(gen) + 0.2);
	glVertex3dv(F);
	glVertex3dv(F1);
	glVertex3dv(G1);
	glVertex3dv(G);
	*/

	//	glColor3d(dis(gen) + 0.2, dis(gen) + 0.4, dis(gen) + 0.5);
	N = vectProz(G, G1, H1, -1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(Gb_texture[0], Gb_texture[1]);
	glVertex3dv(G);
	glTexCoord2d(G1b_texture[0], G1b_texture[1]);
	glVertex3dv(G1);
	glTexCoord2d(H1b_texture[0], H1b_texture[1]);
	glVertex3dv(H1);
	glTexCoord2d(Hb_texture[0], Hb_texture[1]);
	glVertex3dv(H);

	glEnd();






	glBegin(GL_TRIANGLES);

	glColor3d(0.25, 0.15, 0.1);

	N = vectProz(A1, B1, H1, 1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glTexCoord2d(A1_texture[0], A1_texture[1]); 
	glVertex3dv(A1);
	glTexCoord2d(B1_texture[0], B1_texture[1]);
	glVertex3dv(B1);
	glTexCoord2d(H1_texture[0], H1_texture[1]);
	glVertex3dv(H1);


	N = vectProz(H1, B1, C1, 1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(H1_texture[0], H1_texture[1]);
	glVertex3dv(H1);
	glTexCoord2d(B1_texture[0], B1_texture[1]);
	glVertex3dv(B1);
	glTexCoord2d(C1_texture[0], C1_texture[1]);
	glVertex3dv(C1);


	N = vectProz(H1, C1, E1, 1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(H1_texture[0], H1_texture[1]);
	glVertex3dv(H1);
	glTexCoord2d(C1_texture[0], C1_texture[1]);
	glVertex3dv(C1);
	glTexCoord2d(E1_texture[0], E1_texture[1]);
	glVertex3dv(E1);


	N = vectProz(C1, D1, E1, 1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	glTexCoord2d(C1_texture[0], C1_texture[1]);
	glVertex3dv(C1);
	glTexCoord2d(D1_texture[0], D1_texture[1]);
	glVertex3dv(D1);
	glTexCoord2d(E1_texture[0], E1_texture[1]);
	glVertex3dv(E1);

	N = vectProz(H1, E1, F1, 1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	//glVertex3dv(H1);
	//glVertex3dv(E1);
	//glVertex3dv(F1);
	////glVertex3dv(H1);


	N = vectProz(H1, F1, G1, 1);

	iter = N.begin();
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);
	//glVertex3dv(H1);
	//glVertex3dv(F1);
	//glVertex3dv(G1);
	////glVertex3dv(H1);

	glEnd();




	round(0.01, A, B, B1);

	double pointC[] = { -2,-4,0 };

	roundIn(0.01, F, G, pointC, F1, E, H);
}



void round(double step, double pointA[], double pointB[], double pointC[])
{
	glColor3d(0, 0, 0);

	double vect_AB[] = { pointA[0] - pointB[0], pointA[1] - pointB[1] };
	double centre[] = { (pointA[0] + pointB[0]) / 2,  (pointA[1] + pointB[1]) / 2 };

	double length = sqrt(vect_AB[0] * vect_AB[0] + vect_AB[1] * vect_AB[1]);

	double radius = length / 2;

	double Fi = acos(vect_AB[0] / (length + 1));
	//Для верха
	double textur1_vect_AB[] = { A1_texture[0] - B1_texture[0], A1_texture[1] - B1_texture[1] };
	double textur1_centre[] = { (A1_texture[0] + B1_texture[0]) / 2,  (A1_texture[1] + B1_texture[1]) / 2 };

	double textur1_length = sqrt(textur1_vect_AB[0] * textur1_vect_AB[0] + textur1_vect_AB[1] * textur1_vect_AB[1]);

	double textur1_radius = textur1_length / 2;

	//Для низа
	double textur_vect_AB[] = { A_texture[0] - B_texture[0], A_texture[1] - B_texture[1] };
	double textur_centre[] = { (A_texture[0] + B_texture[0]) / 2,  (A_texture[1] + B_texture[1]) / 2 };

	double textur_length = sqrt(textur_vect_AB[0] * textur_vect_AB[0] + textur_vect_AB[1] * textur_vect_AB[1]);

	double textur_radius = textur_length / 2;

	glNormal3d(0, 0, -1);
	glBegin(GL_TRIANGLE_FAN);

	for (double i = -Fi; i <= 3.2 - Fi; i += step)
	{

		double point[] = { radius * cos(i) + centre[0], radius * sin(i) + centre[1], pointA[2] };
		double textur_point[] = { textur_radius * cos(-i+4.115) + textur_centre[0], textur_radius * sin(-i+4.115) + textur_centre[1] };
		glTexCoord2d(textur_point[0], textur_point[1]);
		glVertex3dv(point);

	}



	glEnd();

	glNormal3d(0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);


	for (double i = -Fi; i <= 3.2 - Fi; i += step)
	{

		double point[] = { radius * cos(i) + centre[0], radius * sin(i) + centre[1], pointC[2] };
		double textur1_point[] = {textur1_radius * cos(i) + textur1_centre[0], textur1_radius * sin(i) + textur1_centre[1] };
		glTexCoord2d(textur1_point[0], textur1_point[1]);
		glVertex3dv(point);

	}



	glEnd();


	double counter = (Bb_texture[0] - Ab_texture[0]) /(3.14/step); //Bb_texture[0] >= Ab_texture[0]
	double stepper = counter;
	double oldCounter = 0;
	list<double> N;
	double a, b, c;
	auto iter = N.begin();
	glColor3d(0.2, 0.2, 0.2);

	glBegin(GL_QUADS);


	for (double i = -Fi + 0.065; i <= 3.2 - Fi - step; i += step)
	{
		double point[] = { radius * cos(i) + centre[0], radius * sin(i) + centre[1], pointA[2] };
		double point1[] = { radius * cos(i) + centre[0], radius * sin(i) + centre[1], pointC[2] };
		double point2[] = { radius * cos(i + step) + centre[0], radius * sin(i + step) + centre[1], pointA[2] };
		double point3[] = { radius * cos(i + step) + centre[0], radius * sin(i + step) + centre[1], pointC[2] };

		N = vectProz(point, point2, point3, 1);

		iter = N.begin();
		a = *iter;
		b = *++iter;
		c = *++iter;
		glNormal3d(a, b, c);
		if (Bb_texture[0] >= Ab_texture[0]+counter)
		{
			glTexCoord2d(Ab_texture[0]+oldCounter, Ab_texture[1]);
			glVertex3dv(point);
			glTexCoord2d(A1b_texture[0]+oldCounter, A1b_texture[1]);
			glVertex3dv(point1);
			glTexCoord2d(A1b_texture[0] + counter, A1b_texture[1]);
			glVertex3dv(point3);
			glTexCoord2d(Ab_texture[0] + counter, Ab_texture[1]);
			glVertex3dv(point2);
			counter += stepper;
			oldCounter += stepper;
		}
		else
		{
		
			glVertex3dv(point);
		
			glVertex3dv(point1);
		
			glVertex3dv(point3);
			
			glVertex3dv(point2);
		}
		

	}

	glEnd();
}

void roundIn(double step, double pointA[], double pointB[], double pointC[], double pointA1[], double E[], double H[])
{
	pdd P = make_pair(pointA[0], pointA[1]);
	pdd Q = make_pair(pointB[0], pointB[1]);
	pdd R = make_pair(pointC[0], pointC[1]);
	pdd pointO = findCircumCenter(P, Q, R);

	pdd P_text = make_pair(F1_texture[0], F1_texture[1]);
	pdd Q_text = make_pair(G1_texture[0], G1_texture[1]);
	pdd R_text = make_pair(pointC1_texture[0], pointC1_texture[1]);
	pdd pointO_text = findCircumCenter(P_text, Q_text, R_text);

	pdd PP_text = make_pair(F_texture[0], F_texture[1]);
	pdd QQ_text = make_pair(G_texture[0], G_texture[1]);
	pdd RR_text = make_pair(pointC_texture[0], pointC_texture[1]);
	pdd pointOO_text = findCircumCenter(PP_text, QQ_text, RR_text);

	double c = sqrt(pow((pointB[0] - pointA[0]), 2) + pow((pointB[1] - pointA[1]), 2));//длины
	double a = sqrt(pow((pointC[0] - pointB[0]), 2) + pow((pointC[1] - pointB[1]), 2));//сторон
	double b = sqrt(pow((pointA[0] - pointC[0]), 2) + pow((pointA[1] - pointC[1]), 2));//треугольника
	double s = 0.5 * abs(pointA[0] * (pointB[1] - pointC[1]) - pointA[1] * (pointB[0] - pointC[0]) + pointB[0] * pointC[1] - pointB[1] * pointC[0]);//площадь треуг

	double radius = (a * b * c) / (4 * s);//радиус описанной окр

	double Fi1 = acos((-pointA[0]) / (sqrt(pow(pointA[0], 2) + pow(pointA[1], 2))));

	double Fi2 = acos((-pointB[0]) / (sqrt(pow(pointB[0], 2) + pow(pointB[1], 2))));

	double c_text = sqrt(pow((G1_texture[0] - F1_texture[0]), 2) + pow((G1_texture[1] - F1_texture[1]), 2));//длины
	double a_text = sqrt(pow((pointC1_texture[0] - G1_texture[0]), 2) + pow((pointC1_texture[1] - G1_texture[1]), 2));//сторон
	double b_text = sqrt(pow((F1_texture[0] - pointC1_texture[0]), 2) + pow((F1_texture[1] - pointC1_texture[1]), 2));//треугольника
	double s_text = 0.5 * abs(F1_texture[0] * (G1_texture[1] - pointC1_texture[1]) - F1_texture[1] * (G1_texture[0] - pointC1_texture[0]) + G1_texture[0] * pointC1_texture[1] - G1_texture[1] * pointC1_texture[0]);//площадь треуг

	double radius_text = (a_text * b_text * c_text) / (4 * s_text);//радиус описанной окр

	double cc_text = sqrt(pow((G1_texture[0] - F1_texture[0]), 2) + pow((G1_texture[1] - F1_texture[1]), 2));//длины
	double aa_text = sqrt(pow((pointC1_texture[0] - G1_texture[0]), 2) + pow((pointC1_texture[1] - G1_texture[1]), 2));//сторон
	double bb_text = sqrt(pow((F1_texture[0] - pointC1_texture[0]), 2) + pow((F1_texture[1] - pointC1_texture[1]), 2));//треугольника
	double ss_text = 0.5 * abs(F1_texture[0] * (G1_texture[1] - pointC1_texture[1]) - F1_texture[1] * (G1_texture[0] - pointC1_texture[0]) + G1_texture[0] * pointC1_texture[1] - G1_texture[1] * pointC1_texture[0]);//площадь треуг

	double radius_text_ = (aa_text * bb_text * cc_text) / (4 * ss_text);//радиус описанной окр

	glBegin(GL_TRIANGLE_FAN);

	glNormal3d(0, 0, -1);
	glTexCoord2d(E_texture[0], E_texture[1]);
	glVertex3dv(E);

	for (double i = Fi1 - step - 0.515; i <= Fi2 - step + 0.35; i += step)
	{
		double point[] = { radius * cos(i) + pointO.first, radius * sin(i) + pointO.second, pointA[2] };
		double point_text[] = { radius_text_ * cos(-i + 4.115) + pointOO_text.first, radius_text_ * sin(-i + 4.115) + pointOO_text.second };
		glTexCoord2d(point_text[0], point_text[1]);
		glVertex3dv(point);




	}
	glTexCoord2d(H_texture[0], H_texture[1]);
	glVertex3dv(H);
	glEnd();


	list <double> N;
	auto iter = N.begin();
	glBegin(GL_TRIANGLE_FAN);

	glNormal3d(0, 0, -1);
	glTexCoord2d(H_texture[0], H_texture[1]);
	glVertex3dv(H);

	for (double i = Fi1 - step - 0.515; i <= Fi2 - step + 0.35; i += step)
	{
		double point[] = { radius * cos(i) + pointO.first, radius * sin(i) + pointO.second, pointA[2] };
		double point_text[] = { radius_text_ * cos(-i + 4.115) + pointOO_text.first, radius_text_ * sin(-i + 4.115) + pointOO_text.second };
		glTexCoord2d(point_text[0], point_text[1]);
		glVertex3dv(point);




	}

	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, 1);
	double E1[3];
	E1[0] = E[0];
	E1[1] = E[1];
	E1[2] = pointA1[2];
	glTexCoord2d(E1_texture[0], E1_texture[1]);
	glVertex3dv(E1);

	for (double i = Fi1 - step - 0.515; i <= Fi2 - step + 0.35; i += step)
	{
		double point[] = { radius * cos(i) + pointO.first, radius * sin(i) + pointO.second, pointA1[2] };
		double point_text[] = { radius_text * cos(i) + pointO_text.first, radius_text * sin(i) + pointO_text.second };
		glTexCoord2d(point_text[0], point_text[1]);
		glVertex3dv(point);




	}

	double H1[3];
	H1[0] = H[0];
	H1[1] = H[1];
	H1[2] = pointA1[2];
	glTexCoord2d(H1_texture[0], H1_texture[1]);
	glVertex3dv(H1);
	glEnd();



	glBegin(GL_TRIANGLE_FAN);

	glNormal3d(0, 0, 1);
	E1[0] = E[0];
	E1[1] = E[1];
	E1[2] = pointA1[2];
	glTexCoord2d(H1_texture[0], H1_texture[1]);
	glVertex3dv(H1);

	for (double i = Fi1 - step - 0.515; i <= Fi2 - step + 0.35; i += step)
	{
		double point[] = { radius * cos(i) + pointO.first, radius * sin(i) + pointO.second, pointA1[2] };
		double point_text[] = { radius_text * cos(i) + pointO_text.first, radius_text * sin(i) + pointO_text.second };
		glTexCoord2d(point_text[0], point_text[1]);
		glVertex3dv(point);




	}
	

	glEnd();



	double counter = 0.5*(Gb_texture[0] - Fadd_texture[0]) / (abs(Fi2 - Fi1) / step); 
	double stepper = counter;
	double oldCounter = 0;

	glColor3d(1.0, 0.2, 0.2);

	glBegin(GL_QUADS);

	for (double i = Fi1 - step - 0.515; i <= Fi2 - step + 0.35; i += step)
	{
		double point[] = { radius * cos(i) + pointO.first, radius * sin(i) + pointO.second, pointA[2] };
		double point1[] = { radius * cos(i) + pointO.first, radius * sin(i) + pointO.second, pointA1[2] };
		double point2[] = { radius * cos(i + step) + pointO.first, radius * sin(i + step) + pointO.second, pointA[2] };
		double point3[] = { radius * cos(i + step) + pointO.first, radius * sin(i + step) + pointO.second, pointA1[2] };

		N = vectProz(point, point2, point3, -1);

		iter = N.begin();
		a = *iter;
		b = *++iter;
		c = *++iter;
		glNormal3d(a, b, c);

		if (Gb_texture[0] >= Fadd_texture[0] + counter)
		{
			glTexCoord2d(Fadd_texture[0] + oldCounter, Fadd_texture[1]);
			glVertex3dv(point);
			glTexCoord2d(F1add_texture[0] + oldCounter, F1add_texture[1]);
			glVertex3dv(point1);
			glTexCoord2d(F1add_texture[0] + counter, F1add_texture[1]);
			glVertex3dv(point3);
			glTexCoord2d(Fadd_texture[0] + counter, Fadd_texture[1]);
			glVertex3dv(point2);
			counter += stepper;
			oldCounter += stepper;
		}
		else 
		{
			/*glVertex3dv(point);
			glVertex3dv(point2);
			glVertex3dv(point3);
			glVertex3dv(point1);*/

			glVertex3dv(point);
			glVertex3dv(point1);
			glVertex3dv(point3);
			glVertex3dv(point2);

		}


	}
	glEnd();
}




// Function to find the line given two points
void lineFromPoints(pdd P, pdd Q, double& a, double& b, double& c)
{
	a = Q.second - P.second;
	b = P.first - Q.first;
	c = a * (P.first) + b * (P.second);
}

// Function which converts the input line to its
// perpendicular bisector. It also inputs the points
// whose mid-point lies on the bisector
void perpendicularBisectorFromLine(pdd P, pdd Q, double& a, double& b, double& c)
{
	pdd mid_point = make_pair((P.first + Q.first) / 2,
		(P.second + Q.second) / 2);

	// c = -bx + ay
	c = -b * (mid_point.first) + a * (mid_point.second);

	double temp = a;
	a = -b;
	b = temp;
}

// Returns the intersection point of two lines
pdd lineLineIntersection(double a1, double b1, double c1, double a2, double b2, double c2)
{
	double determinant = a1 * b2 - a2 * b1;
	if (determinant == 0)
	{
		// The lines are parallel. This is simplified
		// by returning a pair of FLT_MAX
		return make_pair(FLT_MAX, FLT_MAX);
	}

	else
	{
		double x = (b2 * c1 - b1 * c2) / determinant;
		double y = (a1 * c2 - a2 * c1) / determinant;
		return make_pair(x, y);
	}
}

pdd findCircumCenter(pdd P, pdd Q, pdd R)
{
	// Line PQ is represented as ax + by = c
	double a, b, c;
	lineFromPoints(P, Q, a, b, c);

	// Line QR is represented as ex + fy = g
	double e, f, g;
	lineFromPoints(Q, R, e, f, g);

	// Converting lines PQ and QR to perpendicular
	// vbisectors. After this, L = ax + by = c
	// M = ex + fy = g
	perpendicularBisectorFromLine(P, Q, a, b, c);
	perpendicularBisectorFromLine(Q, R, e, f, g);

	// The point of intersection of L and M gives
	// the circumcenter
	pdd circumcenter =
		lineLineIntersection(a, b, c, e, f, g);

	if (circumcenter.first == FLT_MAX &&
		circumcenter.second == FLT_MAX)
	{
		cout << "The two perpendicular bisectors "
			"found come parallel" << endl;
		cout << "Thus, the given points do not form "
			"a triangle and are collinear" << endl;
	}

	else
	{
		cout << "The circumcenter of the triangle PQR is: ";
		cout << "(" << circumcenter.first << ", "
			<< circumcenter.second << ")" << endl;
	}


	return circumcenter;
}

void twist()
{
	glScaled(1, 1, 0.02);
	for (int i = 0; i < 50; i++)
	{


		//glRotated(u += 45 * delta_time, 0, 0, 1);
		figure(1);
		glTranslated(0, 0, HEIGHT - 0.12);
		glRotated(1, 0, 0, 0.1);
		figure(1); //перенес рисование фигуры в отдельную функцию 




	}
}

list<double> vectProz(double A[], double B[], double C[], int k)
{
	double A1[] = { B[0] - A[0], B[1] - A[1], B[2] - A[2] };
	double B1[] = { C[0] - A[0], C[1] - A[1], C[2] - A[2] };

	list <double> list_ = { k * (A1[1] * B1[2] - A1[2] * B1[1]), k * (A1[2] * B1[0] - A1[0] * B1[2]), k * (A1[0] * B1[1] - A1[1] * B1[0]) };
	return list_;
}

