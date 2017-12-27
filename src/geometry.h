#ifndef __GEOMETRY_H
#define __GEOMETRY_H

#include <math.h>
#include <vector>
#include <iostream>
#include <ostream>

using namespace std;

class XYZ {

	public:
		float r; // radius
		float r3[3]; // point in space, could be normalized
		float uv[2]; // spherical coordinates, not counting radius

		XYZ();
		XYZ(float x, float y, float z);
		XYZ(const XYZ*);
		~XYZ();

		float getMagnitude() const { return sqrt(r3[0]*r3[0]+r3[1]*r3[1]+r3[2]*r3[2]); }
		void cross(const XYZ&);
		float dot(const XYZ&);
		void normalize();
		void normalize(float f);
		void updateSphericalUV();
		void scale(float s) { r3[0]*=s; r3[1]*=s; r3[2]*=s; }
		void setXYZ(float x,float y, float z) { r3[0]=x; r3[1]=y; r3[2]=z; } 
		void translate(const XYZ *r) { translate(r,1); }
		void translate(const XYZ *r, float scale) { translate(r->r3[0],r->r3[1],r->r3[2],scale); }
		void translate(float x, float y, float z, float scale) {
			x = this->getX() + (x * scale);
			y = this->getY() + (y * scale);
			z = this->getZ() + (z * scale);
			setXYZ(x,y,z);
			updateSphericalUV();
		}

		void rotate(double theta, XYZ *r) {
			float x, y, z;
			float rx, ry, rz;
			double costheta,sintheta;
			r->normalize();
			costheta = cos(theta); sintheta = sin(theta);
			rx = r->getX(); ry = r->getY(); rz = r->getZ();

			x = ((costheta + (1-costheta) * rx * rx) * this->getX()) +
			    (((1-costheta) * rx * ry - rz * sintheta) * this->getY()) +
			    (((1-costheta) * rx * rz + ry * sintheta) * this->getZ());

			y = (((1-costheta) * rx * ry + rz * sintheta) * this->getX()) +
			    ((costheta + (1-costheta) * ry * ry) * this->getY()) +
			    (((1-costheta) * ry * rz - rx * sintheta) * this->getZ());

			z = (((1-costheta) * rx * rz - ry * sintheta) * this->getX()) +
			    (((1-costheta) * ry * rz + rx * sintheta) * this->getY()) +
			    ((costheta + (1-costheta) * rz * rz) * this->getZ());

			setXYZ(x,y,z);
			updateSphericalUV();
		}

		float const getX() const { return r3[0]; }
		float const getY() const { return r3[1]; }
		float const getZ() const { return r3[2]; }
		float const getU() const { return uv[0]; }
		float const getV() const { return uv[1]; }
		float const getR() const { return r; }

		XYZ operator*(float f);
		XYZ operator/(float f);
		XYZ operator+(float f);
		XYZ operator+(const XYZ &p);
		XYZ operator=(const XYZ &p);

		int operator==(const XYZ &p) const;
		XYZ operator+=(const XYZ &p) { translate(&p); return this; }


};
		ostream& operator<<(ostream& out, const XYZ& p);

class Edge {

	public: 

		XYZ *p[3];

		Edge(const Edge*);
		Edge(XYZ *p0, XYZ *p1);
		~Edge();
	
		void split(void);

		XYZ* StartXYZ();
		XYZ* CenterXYZ();
		XYZ* EndXYZ();


};
		ostream& operator<<(ostream& out, const Edge& e);


class Mesh;
class Triangle {
	public:
		Mesh *mesh;
		XYZ *p[3];
		Edge *e[3];
		XYZ normal;

		Triangle(void);
		Triangle(XYZ *p0, XYZ *p1, XYZ *p2);
		Triangle(XYZ *p0, XYZ *p1, XYZ *p2, Mesh *m);
		~Triangle();

		void split(void); // normalize defaults true
		void split(bool normalize, float normsize=1);
		void refresh(XYZ *p0, XYZ *p1, XYZ *p2);

		void calcSphericalUV();

		Triangle& operator=(const Triangle&);
		Triangle& operator=(const Triangle*);

		void scale(double s) {
			p[0]->scale(s);
			p[1]->scale(s);
			p[2]->scale(s);
		}
		void rotate(double theta,XYZ *r) {
			p[0]->rotate(theta,r);
			p[1]->rotate(theta,r);
			p[2]->rotate(theta,r);
		}
		void translate(const XYZ *r) { translate(r,1); }
		void translate(const XYZ *r, float scale) {
			translate(r->r3[0],r->r3[1],r->r3[2],scale);
		}
		void translate(float x, float y, float z) { translate(x,y,z,1); }
		void translate(float x, float y, float z, float scale) {
			p[0]->translate(x,y,z,scale);
			p[1]->translate(x,y,z,scale);
			p[2]->translate(x,y,z,scale);
		}
		XYZ* getAverage() {
			XYZ *a = new XYZ();
			a->setXYZ((p[0]->r3[0]+p[1]->r3[0]+p[2]->r3[0])/3,
				(p[0]->r3[1]+p[1]->r3[1]+p[2]->r3[1])/3,
				(p[0]->r3[2]+p[1]->r3[2]+p[2]->r3[2])/3);
			return a;
		}

};
		ostream& operator<<(ostream& out, const Triangle& t);

class Mesh {
	public:
		vector<Triangle*> triangles;
		vector<Edge*> edges;
		vector<XYZ*> points;

		Mesh();
		Mesh(int lod,float size=1);
		~Mesh();
		const vector<Triangle*> getTriangles();
		Edge* getEdge(XYZ *p1, XYZ *p2);
		float getMaximum() const;
	//protected:
		void subdivide();
		void subdivide(int lod);
		void subdivide(int lod, bool normalize, float normsize = 1);
		void perturb(float f);
		void createIcosahedron();
		void createTetrahedron();
	private:
		float maximum;
};

#endif
