#include <bits/stdc++.h>
#include <windows.h>
#include <gl/glut.h>
#include <cmath>
#define pi (2.0*acos(0.0))
#define INF numeric_limits<double>::infinity()

using namespace std;


int drawgrid;
struct Vector3D
{
	double x,y,z;

};
struct Vector3D pos;
struct Color
{
    double red;
    double green;
    double blue;

    Color() {
        red = green = blue = 0.0;
    }

    Color(double red, double green, double blue) {
        this->red = red;
        this->green = green;
        this->blue = blue;
    }

    ~Color() {
    }
};
struct Coefficients
{

    double ambient,diffuse,specular,reflection_coefficients;
};

struct GeneralCoefficient {
    double a, b, c, d, e, f, g, h, i, j;
};
Vector3D normalize(Vector3D n) {

    double magnitude = sqrt(pow(n.x, 2.0)+pow(n.y, 2.0)+pow(n.z, 2.0));
    n.x = n.x/ magnitude;
    n.y = n.y/ magnitude;
    n.z = n.z/ magnitude;

    return n;
}

class Ray {

    Vector3D start;
    Vector3D dir;

public:
    Ray() {
    }

    Ray(Vector3D start, Vector3D dir) {
        this->start = start;
        this->dir = dir;
    }

    Vector3D getstart() const {
        return start;
    }

    Vector3D getdir() const {
        return normalize(dir);
    }

    void printray(){
        cout << "start : " << start.x << " " << start.y << " " << start.z << endl;
        cout << "dir : " << dir.x << " " << dir.y << " " << dir.z << endl;
    }

    ~Ray() {
    }
};



double dot_product( Vector3D vector_a, Vector3D vector_b) {
   double product = 0;
   product = vector_a.x * vector_b.x+ vector_a.y * vector_b.y + vector_a.z * vector_b.z;
   return product;
}

class Object {
     public:
      //double height, width, length;
      Color color;
      Coefficients coEfficients;
      int shine; // exponent term of specular component



      Object(){
      }
      void setColor(Color color){
          this->color = color;
      }

      Color getColor() const {

        return color;
      }

      void setShine(int shine){

        this->shine=shine;
      }
      int getShine() const {

        return shine;
      }
      void setCoefficients(Coefficients coEfficients){

        this->coEfficients = coEfficients ;
      }

      Coefficients getCoefficients() const {

         return coEfficients;
      }

      Vector3D reference_point;

      virtual void draw() = 0;
      virtual double intersect(Ray,Color&, double) = 0;

      virtual void print(){}

      ~Object() {
    }
};

int recursionLevel = 0;



class Sphere : public Object{

    Vector3D center;
    double radius;

    int slices;
    int stacks;

public:
     Sphere() {
        radius = 0.0;
    }

    Sphere(Vector3D center, double radius) {
        this->center = center;
        this->radius = radius;
    }

void draw()
{
    glTranslatef(center.x,center.y,center.z);
    int stacks = 24;
    int slices = 72;
	Vector3D points[stacks+1][slices+1];
	int i,j;
	double h,r;
	//generate points

	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	glColor3f(getColor().red, getColor().green, getColor().blue);
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f(0,0,1);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}

	glTranslatef(-center.x,-center.y,-center.z);
}

double intersect(Ray ray,Color& Color,double level){

    double tmin,tmax;

    Vector3D starting = ray.getstart();
    starting.x = starting.x-center.x;
    starting.y = starting.y-center.y;
    starting.z = starting.z-center.z;

    //cout<<"r0"<<starting.x<<" "<<starting.y<<" "<<starting.z<<endl;
    //cout<<"rdir"<<ray.getdir().x<<" "<<ray.getdir().y<<" "<<ray.getdir().z<<endl;

    double l_d = - ( ray.getdir().x * starting.x ) - ( ray.getdir().y * starting.y ) - ( ray.getdir().z * starting.z );

    double length = ( starting.x * starting.x ) + ( starting.y * starting.y ) + ( starting.z *starting.z );

    double d = length-pow(l_d,2);

    double temp1,temp2;

    if(l_d<0){

        tmin=INF;
    }
    else{
         temp2 = pow(radius,2);

        if(d>temp2){
            tmin=INF;
        }
        else{
            double l = sqrt(temp2-d);
            tmin = l_d - l;
            tmax = l_d +l;
        }
    }

    double t = NULL;

    if(length<temp2){
        t = tmax;
    }
    else if(length>temp2){
        t = tmin;
    }
    else if(t == NULL ){
        t = min(tmax, tmin);
    }

    Color.red = (color.red > 1.0)? 1.0: ((color.red < 0.0)? 0.0: color.red);
    Color.green = (color.green > 1.0)? 1.0: ((color.green < 0.0)? 0.0: color.green);
    Color.blue = (color.blue > 1.0)? 1.0: ((color.blue < 0.0)? 0.0: color.blue);

    return t;


}
 ~Sphere() {
    }

};



class Floor: public Object

{

  double floorWidth;;
    double tileWidth;

    Color backgroundcolor;

public:
    Floor() {
        floorWidth = tileWidth = 0.0;
    }

    Floor(double floorWidth, double tileWidth, Color backgroundcolor) {
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
        this->backgroundcolor = backgroundcolor;
    }

    Color getColor(Vector3D p)
    {
        int i=floor(p.x/tileWidth);
        int j=floor(p.y/tileWidth);

        return Color((i+j)%2 == 0,(i+j)%2 == 0,(i+j)%2 == 0);
    }



    void draw(){

        int row=(int) floorWidth/tileWidth;
        int column=(int) floorWidth/tileWidth;

        for(int i = 0;i<row;i++){
            for(int j=0; j<column; j++) {
                glColor3f((i+j)%2,(i+j)%2,(i+j)%2);
                //glColor3f(0,0,0);
               // glColor3f(((i+j)%2 == 0)? 0, ((i+j)%2 == 0)? Color().green: backgroundcolor.green, ((i+j)%2 == 0)? getColor().blue: backgroundcolor.blue);
                reference_point.x = -floorWidth/2.0+tileWidth*j;
                reference_point.y = -floorWidth/2.0+tileWidth*i;
                reference_point.z = 0.0;

            glBegin(GL_QUADS);
            {
                glVertex3f(reference_point.x, reference_point.y, reference_point.z);
                glVertex3f(reference_point.x+tileWidth, reference_point.y, reference_point.z);
                glVertex3f(reference_point.x+tileWidth, reference_point.y+tileWidth, reference_point.z);
                glVertex3f(reference_point.x, reference_point.y+tileWidth, reference_point.z);
            }
            glEnd();
        }
    }
}
double intersect(Ray ray,Color& Color,double l_d){


    Vector3D n;
    n.x=0.0;
    n.y=0.0;
    n.z=1.0;
    if(pos.x/n.x > 0.0){
        n.x = n.x;
    }
    else{
        n.x= n.x*(-1.0);
    }
    if(pos.y/n.y > 0.0){
        n.y = n.y;
    }
    else{
        n.y= n.y*(-1.0);
    }
    if(pos.z/n.z > 0.0){
        n.z = n.z;
    }
    else{
        n.z= n.z*(-1.0);
    }

    double t = INF;

    double dot = dot_product(n,ray.getdir());

    if(dot != 0.0) {

        double dot1 = dot_product(n,ray.getstart());
        double dot2 = dot_product(n,ray.getdir());
        double dot3 = dot1/dot2;
        t = (-1.0)*dot3;
    }
Vector3D inter;
    if(t>0.0 && t<INF) {

        inter.x = ray.getstart().x+ray.getdir().x*t;
        inter.y = ray.getstart().y+ray.getdir().y*t;
        inter.z = ray.getstart().z+ray.getdir().z*t;
         Color=getColor(inter);

        if(!((inter.x>-floorWidth/2.0 && inter.x<floorWidth/2.0) && (inter.y>-floorWidth/2.0 && inter.y<floorWidth/2.0)))
        {
            t = INF;
            }

      }


        return t;
        }

    ~Floor() {
    }
};


class Triangle: public Object {
    Vector3D ax;
    Vector3D ay;
    Vector3D az;

public:
    Triangle() {
    }

    Triangle(Vector3D ax, Vector3D ay, Vector3D az) {
        this->ax = ax;
        this->ay = ay;
        this->az = az;
    }

    void print(){
        cout << "Coordinates " << endl;
        cout << ax.x << " " << ax.y << " " << ax.z << endl;
        cout << ay.x << " " << ay.y << " " << ay.z << endl;
        cout << az.x << " " << az.y << " " << az.z << endl;



     }


    void draw(){

    glColor3f(getColor().red, getColor().green, getColor().blue);

    glBegin(GL_TRIANGLES);
    {
        glVertex3f(ax.x, ax.y, ax.z);
        glVertex3f(ay.x, ay.y, ay.z);
        glVertex3f(az.x, az.y, az.z);
    }
    glEnd();

    }
    double intersect(Ray ray,Color& Color,double level){

        Color.red = (color.red > 1.0)? 1.0: ((color.red < 0.0)? 0.0: color.red);
        Color.green = (color.green > 1.0)? 1.0: ((color.green < 0.0)? 0.0: color.green);
        Color.blue = (color.blue > 1.0)? 1.0: ((color.blue < 0.0)? 0.0: color.blue);

        double l1 = ax.x - ay.x;
        double l2 = ax.y - ay.y;
        double l3 = ax.z - ay.z;

        double b1 = az.x - ay.x;
        double b2 = az.y - ay.y;
        double b3 = az.z - ay.z;

        double c1 = - ray.getdir().x;
        double c2 = - ray.getdir().y;
        double c3 = - ray.getdir().z;

        // cout<<ray.getstart().x<<" "<<ray.getstart().y<<ray.getstart().z<<endl;

        double h1 = ray.getstart().x - ay.x ;
        double h2 = ray.getstart().y - ay.y ;
        double h3 = ray.getstart().z - ay.z ;

        double result = l1 * (b2*c3 - c2*b3) + b1 * (c2*l3 - c3*l2) + c1 * (l2*b3 - l3*b2);
        double Q1 = h1 * (b2*c3 - c2*b3) + b1 * (c2*h3 - c3*h2) + c1 * (h2*b3 - h3*b2);
        double Q2 = l1 * (h2*c3 - c2*h3) + h1 * (c2*l3 - c3*l2) + c1 * (l2*h3 - l3*h2);
        double Q3 = l1 * (b2*h3 - h2*b3) + b1 * (h2*l3 - h3*l2) + h1 * (l2*b3 - l3*b2);

        for(int k =0 ;k<level;k++){
            //cout<<k<<endl;
        }

        if( result!= 0 ){
            double before1,before2,t;
            before1 = Q1 / result;
            before2 = Q2 / result;
            t = Q3 / result;

            if(before1 > 0 ) {
               if(before2 > 0){
                if( before1 + before2 <= 1.0 ){
                    return t;
                }
               }
            }

        }

        return -1.0;
    }

    ~Triangle() {

    }
};

class General: public Object {

    GeneralCoefficient co;
    Vector3D refPoints;
    double length;
    double width;
    double height;

public:
    General() {
        co.a = co.b = co.c = co.d = co.e = 0.0;
        co.f = co.g = co.h = co.i = co.j = 0.0;
        length = width = height = 0.0;
    }

    General(GeneralCoefficient co, Vector3D refPoints, double length, double width, double height) {
        this->co = co;
        this->refPoints = refPoints;
        this->length = length;
        this->width = width;
        this->height = height;
    }

    void draw() {
    }

     double intersect(Ray ray,Color& Color,double l_d){

             double a, b, c, t, t2;

    a = co.a*ray.getdir().x*ray.getdir().x+co.b*ray.getdir().y*ray.getdir().y+co.c*ray.getdir().z*ray.getdir().z;
    a += co.d*ray.getdir().x*ray.getdir().y+co.e*ray.getdir().x*ray.getdir().z+co.f*ray.getdir().y*ray.getdir().z;

    b = 2.0*co.a*ray.getstart().x*ray.getdir().x+2.0*co.b*ray.getstart().y*ray.getdir().y+2.0*co.c*ray.getstart().z*ray.getdir().z;
    b += co.d*(ray.getstart().x*ray.getdir().y+ray.getdir().x*ray.getstart().y);
    b += co.e*(ray.getstart().x*ray.getdir().z+ray.getdir().x*ray.getstart().z);
    b += co.f*(ray.getstart().y*ray.getdir().z+ray.getdir().y*ray.getstart().z);
    b += co.g*ray.getdir().x+co.h*ray.getdir().y+co.i*ray.getdir().z;

    c = co.a*ray.getstart().x*ray.getstart().x+co.b*ray.getstart().y*ray.getstart().y+co.c*ray.getstart().z*ray.getstart().z;
    c += co.d*ray.getstart().x*ray.getstart().y+co.e*ray.getstart().x*ray.getstart().z+co.f*ray.getstart().y*ray.getstart().z;
    c += co.g*ray.getstart().x+co.h*ray.getstart().y+co.i*ray.getstart().z+co.j;

    if(a == 0.0) {
            if(b== 0.0){
                t = INF;
            }
            else{
                t = -c/b;
            }
        t2 = INF;
    } else {
        double dom = b*b-4.0*a*c;

        if(dom < 0.0) {
            t = t2 = INF;
        } else if(dom > 0.0) {
            t2 = -b/(2.0*a)+sqrt(dom)/(2.0*a);
            t = -b/(2.0*a)-sqrt(dom)/(2.0*a);
        } else {
            t = -b/(2.0*a);
            t2 = INF;
        }
    }

    if(t < INF) {
        if(t2 < INF) {
            if(t > 0.0) {
                Vector3D inter;
                inter.x = ray.getstart().x+ray.getdir().x*t;
                inter.y = ray.getstart().y+ray.getdir().y*t;
                inter.z = ray.getstart().z+ray.getdir().z*t;

                if((length!=0.0 && (inter.x<refPoints.x || inter.x>refPoints.x+length)) || (width!=0.0 && (inter.y<refPoints.y || inter.y>refPoints.y+width)) || (height!=0.0 && (inter.z<refPoints.z || inter.z>refPoints.z+height))) {
                    t = INF;
                }
            }
            if(t2 > 0.0) {
                Vector3D inter;
                inter.x= ray.getstart().x+ray.getdir().x*t2;
                inter.y= ray.getstart().y+ray.getdir().y*t2;
                inter.z= ray.getstart().z+ray.getdir().z*t2;

                if((length!=0.0 && (inter.x<refPoints.x || inter.x>refPoints.x+length)) || (width!=0.0 && (inter.y<refPoints.y || inter.y>refPoints.y+width)) || (height!=0.0 && (inter.z<refPoints.z || inter.z>refPoints.z+height))) {
                    t2 = INF;
                }
            }
            if(t>0.0 && t<t2){
                t = t;
            }
            else{
                t=t2;
            }
        } else {
            if(t > 0.0) {
                Vector3D inter;
                inter.x= ray.getstart().x+ray.getdir().x*t;
                inter.y= ray.getstart().y+ray.getdir().y*t;
                inter.z= ray.getstart().z+ray.getdir().z*t;

                if((length!=0.0 && (inter.x<refPoints.x || inter.x>refPoints.x+length)) || (width!=0.0 && (inter.y<refPoints.y || inter.y>refPoints.y+width)) || (height!=0.0 && (inter.z<refPoints.z || inter.z>refPoints.z+height))) {
                    t = INF;
                }
            }
        }
    }

        Color.red = (color.red > 1.0)? 1.0: ((color.red < 0.0)? 0.0: color.red);
        Color.green = (color.green > 1.0)? 1.0: ((color.green < 0.0)? 0.0: color.green);
        Color.blue = (color.blue > 1.0)? 1.0: ((color.blue < 0.0)? 0.0: color.blue);

        return t;

    }

    ~General() {

    }
};


class PointLight{

    Vector3D light_pos;
    Color color;

public:
    PointLight(){
    }
    PointLight(Vector3D light_pos,Color color){

        this->light_pos = light_pos;
        this->color = color;
    }
    void draw(){
        glColor3f(color.red, color.green, color.blue);
        glBegin(GL_QUADS);{
            //upper hemisphere
            glVertex3f(light_pos.x, light_pos.y, light_pos.z);
            glVertex3f(light_pos.x + 5, light_pos.y, light_pos.z);
            glVertex3f(light_pos.x + 5, light_pos.y + 5, light_pos.z);
            glVertex3f(light_pos.x, light_pos.y + 5, light_pos.z);
        }glEnd();

    }

    ~PointLight() {
        /* destructor */
    }


};

/*class SpotLight{
    public:
       PointLight point_light;
       Vector3D light_direction;
       double cutoff_angle;

};*/
class SpotLight {

    Vector3D light_pos;
    Color color;
    /* necessary properties for drawing light */
    Vector3D light_direction;
    double cutoff_angle;


public:
    SpotLight() {
        cutoff_angle = 0.0;
    }

    SpotLight(Vector3D light_pos,Color color, Vector3D light_direction,double cutoff_angle) {


        this->light_pos = light_pos;
        this->color = color;
        this->light_direction = light_direction;
        this->cutoff_angle = cutoff_angle;
    }

    void draw(){
        glColor3f(color.red, color.green, color.blue);
        glBegin(GL_QUADS);{
            //upper hemisphere
            glVertex3f(light_pos.x, light_pos.y, light_pos.z);
            glVertex3f(light_pos.x + 5, light_pos.y, light_pos.z);
            glVertex3f(light_pos.x + 5, light_pos.y + 5, light_pos.z);
            glVertex3f(light_pos.x, light_pos.y + 5, light_pos.z);
        }glEnd();

    }

    ~SpotLight() {
        /* destructor */
    }
};

vector<Object*> objects;
vector<SpotLight> spotLights;
vector<PointLight> pointLights;


