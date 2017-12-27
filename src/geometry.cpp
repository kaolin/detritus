#include "geometry.h"

using namespace std;

/** POINT **/
XYZ::XYZ() { }
XYZ::~XYZ() { }

XYZ::XYZ(const XYZ *a) {
  r3[0] = a->r3[0];
  r3[1] = a->r3[1];
  r3[2] = a->r3[2];
}

XYZ::XYZ(float x, float y, float z) {
	r3[0] = x;
	r3[1] = y;
	r3[2] = z;
}

void XYZ::normalize(void) { XYZ::normalize(1); }
void XYZ::normalize(float f) {
	r = sqrt(r3[0]*r3[0]+r3[1]*r3[1]+r3[2]*r3[2]);
	if (r == 0) {
		r3[0] = r3[1] = r3[2] = 0;
	} else {
		r3[0]/=r;
		r3[1]/=r;
		r3[2]/=r;
	}
	r3[0]*=f;
	r3[1]*=f;
	r3[2]*=f;
}

void XYZ::cross(const XYZ& a) {
	float x, y, z;
	x = r3[1] * a.r3[2] - r3[2] * a.r3[1];
	y = -1 * (r3[0] * a.r3[2] - r3[2] * a.r3[0]);
	z = r3[0] * a.r3[1] - r3[1] * a.r3[0];
	r3[0] = x;
	r3[1] = y;
	r3[2] = z;
}

float XYZ::dot(const XYZ& a) {
	return r3[0]*a.r3[0] + r3[1]*a.r3[1] + r3[2]*a.r3[2];
}

void XYZ::updateSphericalUV(void) {
	float x,y,z;

	x = r3[0]; y = r3[1]; z = r3[2];

	r = sqrt(x*x+y*y+z*z); // rho
	uv[0] = (atan2(x,z)/(2*M_PI)) + .5; // theta, longitude
	uv[1] = acos(y/r)/M_PI; //phi, latitude

}

XYZ XYZ::operator*(float f) {
	return XYZ(this->r3[0]*f,this->r3[1]*f,this->r3[2]*f);
}

XYZ XYZ::operator/(float f) {
	return XYZ(this->r3[0]/f,this->r3[1]/f,this->r3[2]/f);
}

XYZ XYZ::operator+(float f) {
	return XYZ(this->r3[0]+f,this->r3[1]+f,this->r3[2]+f);
}

XYZ XYZ::operator+(const XYZ &p) {
	return XYZ(this->r3[0]+p.r3[0],this->r3[1]+p.r3[1],this->r3[2]+p.r3[2]);
}

XYZ XYZ::operator=(const XYZ &p) {
	r3[0] = p.r3[0];
	r3[1] = p.r3[1];
	r3[2] = p.r3[2];

	uv[0] = p.uv[0];
	uv[1] = p.uv[1];
	return *this;
}

int XYZ::operator==(const XYZ &p) const {
	return (r3[0] == p.r3[0] && r3[1] == p.r3[1] && r3[2] == p.r3[2]);
}

ostream& operator<<(ostream& out,const XYZ &p) {
	out<< "[ " << p.r3[0] << "," << p.r3[1] << "," << p.r3[2];
	//out "; " << p.uv[0] << "," << p.uv[1] ;
	out << "] ";
	return out;
}


/** end point **/

/** EDGE **/
Edge::Edge(const Edge *a) {
  p[0]=a->p[0];
  p[1]=a->p[1];
  p[2]=a->p[2];
}
Edge::Edge(XYZ* start, XYZ* end) {
	p[0]=start; p[1] = NULL; p[2] = end;
}

Edge::~Edge() {  }

void Edge::split() {
	float x, y, z;
	if (p[1]) return;
	x  = (p[0]->r3[0] + p[2]->r3[0])*.5;
	y  = (p[0]->r3[1] + p[2]->r3[1])*.5;
	z  = (p[0]->r3[2] + p[2]->r3[2])*.5;
	p[1] = new XYZ(x,y,z);
}

XYZ* Edge::StartXYZ() { return p[0]; }
XYZ* Edge::CenterXYZ() { if (p[1]) return p[1]; split(); return p[1]; }
XYZ* Edge::EndXYZ() { return p[2]; }

ostream& operator<<(ostream& out,const Edge &e) {
	out << *(e.p[0]) << endl;
	out << "[[ " << *(e.p[0]) << " to " << *(e.p[2]);
	if (e.p[1]) { out << "( " << *(e.p[2]) << " )"; }
	out << " ]] ";
	return out;
}

/** end edge **/

/** TRIANGLE **/
Triangle::Triangle(void) {
	mesh = NULL;
	p[0] = p[1] = p[2] = NULL;
}

Triangle::Triangle(XYZ *p0, XYZ *p1, XYZ *p2) {
	mesh = NULL;
	refresh(p0,p1,p2);
}

Triangle::Triangle(XYZ *p0, XYZ *p1, XYZ *p2, Mesh *m) {
	mesh = m;
	refresh(p0,p1,p2);
}

Triangle::~Triangle() {  }

void Triangle::refresh(XYZ *p0, XYZ *p1, XYZ *p2) {
	p[0]=p0; p[1] = p1; p[2] = p2;
	if (!mesh) {
		e[0]=new Edge(p0,p1);
		e[1]=new Edge(p1,p2);
		e[2]=new Edge(p2,p0);
	} else {
		e[0]=mesh->getEdge(p0,p1);
		e[1]=mesh->getEdge(p1,p2);
		e[2]=mesh->getEdge(p2,p0);
	}
}

void Triangle::calcSphericalUV() {
	for (int i=0;i<3;i++) p[i]->updateSphericalUV();
}

void Triangle::split() { split(true); }

void Triangle::split(bool normalize, float normsize) {
	XYZ *a, *b, *c, *d, *e, *f;

	a = p[0];
	b = p[1];
	c = p[2];

	d = this->e[0]->CenterXYZ();
	e = this->e[1]->CenterXYZ();
	f = this->e[2]->CenterXYZ();

	if (normalize) {
		a->normalize(normsize);
		b->normalize(normsize);
		c->normalize(normsize);
		d->normalize(normsize);
		e->normalize(normsize);
		f->normalize(normsize);
	}

	refresh(a,d,f);
	mesh->triangles.push_back(new Triangle(d,b,e,mesh));
	mesh->triangles.push_back(new Triangle(f,e,c,mesh));
	mesh->triangles.push_back(new Triangle(d,e,f,mesh));

}

/* CHEAP copy constructor, not deep! */
Triangle& Triangle::operator=(const Triangle& t) {
	mesh = NULL; // TODO: deep copy!?
	normal = t.normal;
	for (int i=0;i<3;i++) {
		p[i] = new XYZ(t.p[i]);
		e[i] = new Edge(t.e[i]);
	}
	return *this;
}

Triangle& Triangle::operator=(const Triangle *t) {
	mesh = NULL; // TODO: deep copy!? TODO: really, new new new???
	normal = t->normal;
	for (int i=0;i<3;i++) {
		p[i] = new XYZ(t->p[i]);
		e[i] = new Edge(t->e[i]);
	}
	return *this;
}

ostream& operator<<(ostream& out, const Triangle& t) {
	out << "TRI: " << endl;
	out << "  " << *(t.p[0]) << ", " << *(t.p[1]) << ", " << *(t.p[2]) << endl;
	out << "  " << *(t.e[0]) << endl;
	out << "  " << *(t.e[1]) << endl;
	out << "  " << *(t.e[2]) << endl << endl;
	return out;
}

/** end triangle **/

/** MESH **/
Mesh::Mesh() {
	//create one triangle :)
	//triangles. huh??
	createIcosahedron();
}

Mesh::Mesh(int lod,float size) {
	triangles.reserve(10000);
	createTetrahedron();
	subdivide(lod,true,size);
	unsigned it = triangles.size();
	Triangle *t;
	for (unsigned i=0;i<it;i++) {
		t = triangles[i];
		t->calcSphericalUV();
	}
}

Mesh::~Mesh() {
	cout << "MESH DESTROYED" << endl;
}

Edge* Mesh::getEdge(XYZ *p1, XYZ *p2) {
	unsigned it = edges.size();
	Edge *e;
	for (unsigned i=0;i<it;i++) {
		e = edges[i];
		if ( (e->p[0] == p1 && e->p[2] == p2) || (e->p[2] == p1 && e->p[0] == p2)) {
			return edges[i];
		}
	}
	e = new Edge(p1,p2);
	edges.push_back(e);
	return e;
}

const vector<Triangle*> Mesh::getTriangles() {
	return triangles;
}

//presumption: we're starting with a happy little collection of
//triangles that are already smart about sharing edges.  Maybe they
//are, now.  I hope...  So a mesh... should keep a list of all
//available edges... as they're created. anyway... subdividing...
void Mesh::subdivide() { subdivide(1,true); }
void Mesh::subdivide(int lod) { subdivide(lod,true); }
void Mesh::subdivide(int lod, bool normalize, float normsize) {
	for (;lod > 0;lod--) {
		unsigned origSize = triangles.size();
		for (unsigned i = 0 ; i < origSize ; ++i) {
			Triangle *t = triangles[i];
			t->split(normalize,normsize);
		}
#if 0
		origSize = edges.size();
		for (unsigned i = 0 ; i < origSize ; ++i) {
			Edge *e = edges[i];
			//*e[1] = NULL; // clean up the splits so we're not confused later
		}
#endif
	}
	unsigned it = triangles.size();
	for (unsigned i=0;i<it;i++) {
		Triangle *t = triangles[i];
		t->calcSphericalUV();
	}
}

void Mesh::perturb(float f) {
	vector<XYZ*> finder;
#if 0
	unsigned origSize = edges.size();
	for (unsigned i = 0 ; i < origSize ; ++i) {
		Edge *e = edges[i];
		XYZ *p0 = e->StartXYZ();
		XYZ *p1 = e->EndXYZ();
		unsigned jSize = finder.size();
		bool found = false;
		for (unsigned j = 0; j < jSize && !found; j++) {
			found = (*finder[j] == *p0);
		}
		if (!found) {
			//if (p0->r3[0] > 0) { p0->r3[0]+=.4; } else { p0->r3[0]-=.4; }
			p0->r3[0] += 7; // f * (p0->r3[0] > 0)?1: -1;//*rand()/RAND_MAX;
			p0->r3[1] += 7; // f * (p0->r3[1] > 0)?1: -1;//*rand()/RAND_MAX;
			p0->r3[2] += 7; // f * (p0->r3[2] > 0)?1: -1;//*rand()/RAND_MAX;
			finder.push_back(p0);
		} else {
			p0->r3[0] += 7; // f * (p0->r3[0] > 0)?1: -1;//*rand()/RAND_MAX;
			p0->r3[1] += 7; // f * (p0->r3[1] > 0)?1: -1;//*rand()/RAND_MAX;
			p0->r3[2] += 7; // f * (p0->r3[2] > 0)?1: -1;//*rand()/RAND_MAX;
		}
		jSize = finder.size();
		found = false;
		for (unsigned j = 0; j < jSize && !found; j++) {
			found = (*finder[j] == *p1);
		}
		if (!found) {
			//if (p1->r3[0] > 0) { p1->r3[0]+=.4; } else { p1->r3[0]-=.4; }
			p1->r3[0] += f*rand()/RAND_MAX;
			p1->r3[1] += f*rand()/RAND_MAX;
			p1->r3[2] += f*rand()/RAND_MAX;
			finder.push_back(p1);
		}
	}
	unsigned it = triangles.size();
	for (unsigned i=0;i<it;i++) {
		Triangle *t = triangles[i];
		t->calcSphericalUV();
	}
#endif
	unsigned it = triangles.size();
	for (unsigned i=0;i<it;i++) {
		Triangle *t = triangles[i];
		for (int j=0;j<3;j++) {
			Edge *e = t->e[j];
			XYZ *p0 = e->StartXYZ();
			XYZ *p1 = e->EndXYZ();
			unsigned jSize = finder.size();
			bool found = false;
			for (unsigned j = 0; j < jSize && !found; j++) {
				found = (*finder[j] == *p0);
			}
			if (!found) {
				p0->r3[0] += f *rand()/RAND_MAX;
				p0->r3[1] += f *rand()/RAND_MAX;
				p0->r3[2] += f *rand()/RAND_MAX;
				finder.push_back(p0);
			}
			jSize = finder.size();
			found = false;
			for (unsigned j = 0; j < jSize && !found; j++) {
				found = (*finder[j] == *p1);
			}
			if (!found) {
				//if (p1->r3[0] > 0) { p1->r3[0]+=.4; } else { p1->r3[0]-=.4; }
				p1->r3[0] += f*rand()/RAND_MAX;
				p1->r3[1] += f*rand()/RAND_MAX;
				p1->r3[2] += f*rand()/RAND_MAX;
				finder.push_back(p1);
			}
		}
		t->calcSphericalUV();
	}
}

/** http://documents.wolfram.com/p5/Add-onsLinks/StandardPackages/Graphics/Polyhedra.html **/
void Mesh::createTetrahedron() {
    
    // build a tetrahedron
    
    float sqrt_2_3 = sqrt(2.0/3.0);
    float sqrt_3 = sqrt(3.0);
		float sqrt_2 = sqrt(2.0);

    // create the 4 vertices
    XYZ *p0 = new XYZ(0,0,sqrt_3);
    XYZ *p1 = new XYZ(0,2*sqrt_2_3,-1/sqrt_3);
    XYZ *p2 = new XYZ(-sqrt_2,-sqrt_2_3,-1/sqrt_3);
    XYZ *p3 = new XYZ(sqrt_2,-sqrt_2_3,-1/sqrt_3);

		p0->normalize();
		p1->normalize();
		p2->normalize();
		p3->normalize();
    
    // create the 4 triangles
    triangles.push_back(new Triangle(p0,p1,p2,this));
    triangles.push_back(new Triangle(p0,p2,p3,this));
    triangles.push_back(new Triangle(p0,p3,p1,this));
    triangles.push_back(new Triangle(p1,p3,p2,this));
   
}

/* http://en.wikipedia.org/wiki/Icosahedron */
/* http://home.att.net/~numericana/answer/polyhedra.htm#platonic */
void Mesh::createIcosahedron() {
    
		vector<Triangle*> triangles;
    // build an icosahedron
    
    float t = (1 + sqrt(5.0))/2.0;
    float s = 1/sqrt(1 + t*t);
		t*=s;

    // create the 12 vertices
    XYZ *p0 = new XYZ(t, s, 0);
    XYZ *p1 = new XYZ(-t, s, 0);
    XYZ *p2 = new XYZ(t, -s, 0);
    XYZ *p3 = new XYZ(-t, -s, 0);
    XYZ *p4 = new XYZ(s, 0, t);
    XYZ *p5 = new XYZ(s, 0, -t);
    XYZ *p6 = new XYZ(-s, 0, t);
    XYZ *p7 = new XYZ(-s, 0, -t);
    XYZ *p8 = new XYZ(0, t, s);
    XYZ *p9 = new XYZ(0, -t, s);
    XYZ *p10 = new XYZ(0, t, -s);
    XYZ *p11 = new XYZ(0, -t, -s);
    
    // create the 20 triangles
    triangles.push_back(new Triangle(p0, p8, p4,this));
    triangles.push_back(new Triangle(p1, p10, p7,this));
    triangles.push_back(new Triangle(p2, p9, p11,this));
    triangles.push_back(new Triangle(p7, p3, p1,this));    
    triangles.push_back(new Triangle(p0, p5, p10,this));
    triangles.push_back(new Triangle(p3, p9, p6,this));
    triangles.push_back(new Triangle(p3, p11, p9,this));
    triangles.push_back(new Triangle(p8, p6, p4,this));    
    triangles.push_back(new Triangle(p2, p4, p9,this));
    triangles.push_back(new Triangle(p3, p7, p11,this));
    triangles.push_back(new Triangle(p4, p2, p0,this));
    triangles.push_back(new Triangle(p9, p4, p6,this));    
    triangles.push_back(new Triangle(p2, p11, p5,this));
    triangles.push_back(new Triangle(p0, p10, p8,this));
    triangles.push_back(new Triangle(p5, p0, p2,this));
    triangles.push_back(new Triangle(p10, p5, p7,this));    
    triangles.push_back(new Triangle(p1, p6, p8,this));
    triangles.push_back(new Triangle(p1, p8, p10,this));
    triangles.push_back(new Triangle(p6, p1, p3,this));
    triangles.push_back(new Triangle(p11, p7, p5,this));
    
		this->triangles = triangles;
    
}

float Mesh::getMaximum() const {
//cout << "IN GET MAXIMUM" << endl;
//cout << "TRIANGLES REF: " << &triangles << endl;
	int it;
	float maximum = 0, temp;
	it = triangles.size();
//if (it > 100) return 0; // TODO: FUCK, REMOVE
//cout << "it: " << it << endl;
	for (int i=0;i<it;i++) {
		Triangle *t = triangles[i];
//cout << "Triangle t: " << t << endl;
		for (int j=0;j<3;j++) {
//cout << "j: " << j << endl;
			temp = t->p[j]->getMagnitude();
//cout << "temp: " << temp << endl;
			if (temp > maximum) maximum = temp;
		}
	}
//cout << "RETURNING: " << maximum << endl;
	return maximum;
}

/** end mesh **/
