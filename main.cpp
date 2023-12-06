#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#include<fstream>
#include<ostream>
#include <windows.h>
#include <gl/glut.h>

#include "1705052_classes.hpp"
#include "bitmap_image.hpp"


using namespace std;

#define Max_length 25.0
#define pi 3.1416

double square_length;
double radius_length;

double cameraHeight;
double cameraAngle;
int drawaxes;
double angle;
extern int recursionLevel;
int Dimension = 0;
int objectsCount = 0;
int pointLightSource = 0;
int spotLightSource = 0;
int windowWidth = 500;
int windowHeight = 500;
double viewAngle= 80.0;

extern vector <Object*> objects;
extern vector <PointLight> pointLights;
extern vector <SpotLight> spotLights;

struct point
{
	double x,y,z;

};
extern struct Vector3D pos;
struct point u;
struct point r;
struct point l;

int bitmapImage;



void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}

void capture()
{
    bitmap_image image(Dimension,Dimension);
    glColor3f(0,0,0);

    for(int i=0; i<Dimension; i++) {
        for(int j=0; j<Dimension; j++) {
            image.set_pixel(i,j , 0, 0, 0);
        }
    }

    double planeDistance = ((double)(windowHeight)/2.0) / tan((viewAngle*pi)/(2.0*180.0));  //windowHeight/(2.0*tan(viewAngle/2.0*pi/180.0));
    // cout<<"planedis:"<<planeDistance<<endl;
    Vector3D topleft;
    topleft.x = pos.x + (l.x*planeDistance) - r.x*(windowWidth/2) +u.x*(windowHeight/2);
    topleft.y = pos.y + (l.y*planeDistance) - r.y*(windowWidth/2) +u.y*(windowHeight/2);
    topleft.z = pos.z + (l.z*planeDistance) - r.z*(windowWidth/2) +u.z*(windowHeight/2);

    double du =  ((double) windowWidth/Dimension);
    double dv =  ((double) windowHeight/Dimension);

    topleft.x = topleft.x + r.x*(0.5*du) - u.x*(0.5*dv);
    topleft.y = topleft.y + r.y*(0.5*du) - u.y*(0.5*dv);
    topleft.z = topleft.z + r.z*(0.5*du) - u.z*(0.5*dv);

    for(int i = 0; i<Dimension; i++) {
        for(int j = 0; j<Dimension; j++) {

            Vector3D curPixel;
             curPixel.x = topleft.x+r.x*(i*du)-u.x*(j*dv);
             curPixel.y = topleft.y+r.y*(i*du)-u.y*(j*dv);
             curPixel.z = topleft.z+r.z*(i*du)-u.z*(j*dv);

            Vector3D cast_ray;
            cast_ray.x = curPixel.x-pos.x;
            cast_ray.y = curPixel.y-pos.y;
            cast_ray.z = curPixel.z-pos.z;

            Ray ray(pos,cast_ray);
            // ray.printray();
            int closest=INT_MAX;
            double t = INF;
            double t_min = INF;

            for(int p=0; p<objects.size(); p++) {
                Color color;
                t = objects[p]->intersect(ray, color, 0);
                //image.set_pixel(i, j, (int) round(color.red*255.0), (int) round(color.green*255.0), (int) round(color.blue*255.0));

                if(t>0.0 && t<t_min) {
                    t_min = t;
                    closest  = p;
                }
            }

            if(closest != INT_MAX) {
                // cout << closest << endl;
                Color color;
                t_min = objects[closest]->intersect(ray, color, 1);
               //  cout << color.red << " " << color.green << " " << color.blue << endl;
                image.set_pixel(i, j, (int) round(color.red*255.0), (int) round(color.green*255.0), (int) round(color.blue*255.0));
            }

        }
    }

    stringstream Count;
    Count << (++bitmapImage);

    image.save_image("D:\\L4T1\\CSE_410\\Assignment 3\\1705052\\output"+Count.str()+".bmp");

}

void LoadData()
{

    Object* temp = NULL;

    Color c_;
    c_.red = 1;
    c_.green = 1;
    c_.blue = 1;
    temp = new Floor(1000,20,c_);
    objects.push_back(temp);


    ifstream fin;
    fin.open("D:\\L4T1\\CSE_410\\Assignment 3\\1705052\\scene.txt",ios::in);

    if(!fin.is_open()) {
        cout << "input.is_open(): failed to open input file" << endl;
        exit(EXIT_FAILURE);
    }

    fin >> recursionLevel >> Dimension;

    fin >> objectsCount;


    string shape;

    for(int i=0; i<objectsCount; i++) {

        fin >> shape;
        //cout<<shape<<endl;

        if(shape.compare("sphere") == 0) {

            Vector3D center;
            double radius;

            fin >> center.x >>center.y>>center.z;
            fin >> radius;

            temp = new Sphere(center, radius);

        }
         else if(shape.compare("triangle") == 0) {
            cout << "triangle " << endl;
            Vector3D i, j, k;

            fin >> i.x>>i.y>>i.z;
            fin >> j.x>>j.y>>j.z;
            fin >> k.x>>k.y>>k.z;

            temp = new Triangle(i, j, k);
            temp->print();
        }
        else if(shape.compare("general") == 0) {

            GeneralCoefficient co;
            Vector3D refPoints;
            double length, width, height;

            fin >> co.a>>co.b>>co.c>>co.d>>co.e>>co.f>>co.g>>co.h>>co.i>>co.j;
            fin >> refPoints.x>>refPoints.y>>refPoints.z;
            fin >> length >> width >> height;

            temp = new General(co, refPoints, length, width, height);
        }

        Color color;
        Coefficients coefficient;
        int shIne;

        fin >> color.red>>color.green>>color.blue;
        fin>> coefficient.ambient>>coefficient.diffuse>>coefficient.specular>>coefficient.reflection_coefficients;
        fin >> shIne;

        temp->setColor(color);
        temp->setCoefficients(coefficient);
        temp->setShine(shIne);

        objects.push_back(temp);

    }

        //cout<<objects.size()<<endl;
        temp = NULL;

        fin >> pointLightSource;

       for(int i=0; i<pointLightSource; i++) {

        Vector3D position;
        Color color;

        fin >> position.x>>position.y>>position.z;
        fin >> color.red>>color.green>>color.blue;

        pointLights.push_back(PointLight(position, color));
      }

      fin>>spotLightSource;

      for(int i=0; i<spotLightSource; i++) {

        Vector3D position;
        Color color;
        Vector3D light_direction;
        double cutoff_angle;

        fin >> position.x>>position.y>>position.z;
        fin >> color.red>>color.green>>color.blue;
        fin >> light_direction.x >> light_direction.y >> light_direction.z ;
        fin >>cutoff_angle;

        spotLights.push_back(SpotLight(position, color,light_direction,cutoff_angle));
      }
    fin.close();


}

void keyboardListener(unsigned char key, int x,int y){
    double tx,ty,tz;
	switch(key){
        case '0':
            {
                capture();
            }
            break;

        case '1':
			{
                double arg = (-2) * (pi / 180.0);
                double s_ = sin( arg );
                double c_ = cos( arg );
                tx  = l.x;
                ty  = l.y;
                tz  = l.z;
                l.x = l.x*(c_)+r.x * (s_);
                l.y = l.y*(c_)+r.y * (s_);
                l.z = l.z*(c_)+r.z * (s_);

                r.z = r.z*(c_)-tz * (s_);
                r.x = r.x*(c_)-tx * (s_);
                r.y = r.y*(c_)-ty * (s_);

			}
			break;
        case '2':
			{
			    double arg = 2 * (pi / 180.0);
                double s_ = sin( arg );
                double c_ = cos( arg );
                tx  = l.x;
                ty  = l.y;
                tz  = l.z;

                l.x = l.x*(c_)+r.x * (s_);
                l.y = l.y*(c_)+r.y * (s_);
                l.z = l.z*(c_)+r.z * (s_);

                r.z = r.z*(c_)-tz * (s_);
                r.x = r.x*(c_)-tx * (s_);
                r.y = r.y*(c_)-ty * (s_);


			}
			break;
	    case '3':
			{
                double arg = (-2) * (pi / 180.0);
                double s_ = sin( arg );
                double c_ = cos( arg );
                tx  = u.x;
                ty  = u.y;
                tz  = u.z;

                u.x = u.x*(c_)+l.x * (s_);
                u.y = u.y*(c_)+l.y * (s_);
                u.z = u.z*(c_)+l.z * (s_);

                l.z = l.z*(c_)-tz * (s_);
                l.x = l.x*(c_)-tx * (s_);
                l.y = l.y*(c_)-ty * (s_);
			}
			break;
        case '4':
			{
                double arg = 2 * (pi / 180.0);
                double s_ = sin( arg );
                double c_ = cos( arg );
                tx  = u.x;
                ty  = u.y;
                tz  = u.z;
                u.x = u.x*(c_)+l.x * (s_);
                u.y = u.y*(c_)+l.y * (s_);
                u.z = u.z*(c_)+l.z * (s_);
                l.z = l.z*(c_)-tz * (s_);
                l.x = l.x*(c_)-tx * (s_);
                l.y = l.y*(c_)-ty * (s_);
			}
			break;

		case '5':
			{
                double arg = (-5) * (pi / 180.0);
                double s_ = sin( arg );
                double c_ = cos( arg );
                tx  = r.x;
                ty  = r.y;
                tz  = r.z;
                r.x = r.x*(c_)+u.x * (s_);
                r.y = r.y*(c_)+u.y * (s_);
                r.z = r.z*(c_)+u.z * (s_);
                u.z = u.z*(c_)-tz * (s_);
                u.x = u.x*(c_)-tx * (s_);
                u.y = u.y*(c_)-ty * (s_);
			}
			break;
        case '6':
            {
                double arg = (5) * (pi / 180.0);
                double s_ = sin( arg );
                double c_ = cos( arg );
                tx  = r.x;
                ty  = r.y;
                tz  = r.z;
                r.x = r.x*(c_)+u.x * (s_);
                r.y = r.y*(c_)+u.y * (s_);
                r.z = r.z*(c_)+u.z * (s_);
                u.z = u.z*(c_)-tz * (s_);
                u.x = u.x*(c_)-tx * (s_);
                u.y = u.y*(c_)-ty * (s_);
			}

            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			//cameraHeight -= 3.0;
			pos.z -= l.z;
			pos.y -= l.y;
			pos.x -= l.x;
			break;
		case GLUT_KEY_UP:		// up arrow key
			//cameraHeight += 3.0;
			pos.z += l.z;
			pos.y += l.y;
			pos.x += l.x;
			break;

		case GLUT_KEY_RIGHT:
			pos.y += r.y;
			pos.x += r.x;
			pos.z += r.z;
			break;
		case GLUT_KEY_LEFT:
			pos.y -= r.y;
			pos.x -= r.x;
			pos.z -= r.z;
			break;

		case GLUT_KEY_PAGE_UP:
		    pos.z += u.z;
			pos.y += u.y;
			pos.x += u.x;
		    //cameraHeight += 3.0;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pos.z -= u.z;
			pos.y -= u.y;
			pos.x -= u.x;
			break;
		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
		    if(square_length>0){
               square_length--;
		    }

			break;
		case GLUT_KEY_END:
		    if(square_length<Max_length){
                square_length++;
		    }

			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

    radius_length = Max_length - square_length;
	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	gluLookAt(pos.x, pos.y, pos.z,     pos.x + l.x, pos.y + l.y, pos.z + l.z,     u.x, u.y, u.z);
	glMatrixMode(GL_MODELVIEW);

	drawAxes();
	//drawGrid();

    glColor3f(0,0,0);

    for(int i=0; i<objects.size(); i++) {
        objects[i]->draw();
	}
	for(int i=0; i<pointLights.size(); i++) {
        pointLights[i].draw();
	}
	for(int i=0; i<spotLights.size(); i++) {
        spotLights[i].draw();
	}


    //glFlush();
	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
    pos.x = 0;
    pos.y = 0;
    pos.z = -100;
    u.x = 0;
    u.y = 1;
    u.z = 0;
    r.x = -1;
    r.y = 0;
    r.z = 0;
    l.x = 0;
    l.y = 0;
    l.z = 1;

	//clear the screen
	glClearColor(0,0,0,0);

	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);


}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();
    square_length = 15.0;
    radius_length = Max_length - square_length;
	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	LoadData();
//    cout << "before loop " << endl;
//    cout << objects.size() << endl;
//	for(int i = 0 ; i < objects.size() ; i++){
//        objects[i]->print();
//	}

	glutMainLoop();		//The main loop of OpenGL

	for(int i=0; i<objects.size(); i++) {
        delete objects[i];
    }
    objects.clear();
    spotLights.clear();
    pointLights.clear();

	return 0;
}
