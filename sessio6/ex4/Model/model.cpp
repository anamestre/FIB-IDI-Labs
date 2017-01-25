/*
 *  model.cpp
 *  (c) 2012 ViRVIG (http://www.virvig.eu)
 *  This source code is licensed under the creative commons CC BY-NC-SA 3.0
 *  license (see http://creativecommons.org/licenses/by-nc-sa/3.0/)
 *
 */

#define __MODEL__DEF__ 1
#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cassert>
using namespace std;
// === Local stuff:
static int material = 1;
static void loadMTL(std::string filename);
static int findMat(string material);
static void omplenormals(vector<Face> &_faces, 
			 vector<Vertex> const &_vertices);
static void ompleVBOs(vector<Face> &_faces, 
	              vector<Vertex> const &_vertices,
	              vector<Normal> const &_normals,
		      float *&_VBO_vert, float *&_VBO_norm,
		      float *&_VBO_mata, float *&_VBO_matd, float *&_VBO_matsp, float *&_VBO_matsh);

static bool fvtn = false;
static bool fvt = false;
static bool texcoord = false;
static string modelPath("");

// ======== Constructors and Destructors =======
Model::Model() : _vertices(0), _normals(0), _faces(0) {
  _VBO_vertices = _VBO_normals = _VBO_matamb = _VBO_matdiff = _VBO_matspec = _VBO_matshin = NULL;
}

Model::~Model() {
  if (_VBO_vertices != NULL) delete _VBO_vertices;
  if (_VBO_normals != NULL) delete _VBO_normals;
  if (_VBO_matamb != NULL) delete _VBO_matamb;
  if (_VBO_matdiff != NULL) delete _VBO_matdiff;
  if (_VBO_matspec != NULL) delete _VBO_matspec;
  if (_VBO_matshin != NULL) delete _VBO_matshin;
}

Material::Material() : name("__load_object_default_material__") {
  ambient[0] = ambient[1] = ambient[2] = 0.1; ambient[3] = 1.0;
  diffuse[0] = diffuse[1] = 0.7; diffuse[2] = 0.0; diffuse[3] = 1.0;
  specular[0] = specular[1] = specular[2] = 1.0; specular[3] = 1.0;
  shininess = 64;
}

// ========= Public methods ==========
void Model::load(std::string filename) {
  if (! _vertices.empty()) {
    // unload previous model:
    _vertices.erase(_vertices.begin(), _vertices.end());
    _normals.erase(_normals.begin(), _normals.end());
    _faces.erase(_faces.begin(), _faces.end());
  }
  size_t fiPath = filename.rfind("/");
  if (fiPath == string::npos) modelPath = "";
  else modelPath = filename.substr(0, fiPath+1);

  fstream input(filename.data(), ios::in);
  if (input.rdstate() != ios::goodbit) {
    cerr << "Cannot load OBJ file " << filename << endl;
    return;
  }
  string line;
  stringstream ss;
  while (getline(input, line)) {
#if DEBUGPARSER
    cerr << "Just read '" << line << "'" << endl;
#endif
    ss.clear(); ss.str(line);
    char c = '#'; // to skip whitelines...
    ss >> c;
    // The following variables need to be declared before entering the switch.
    // They are intended for use in the different cases...
    string tail;
    double coord;
    Face auxFace;
    stringstream auxss;
    size_t first, second;
    switch(c){
      //-------------
    case '#':    // comment line
      break;
      //-------------
    case 'v':    // vertex information...
      ss >> noskipws >> c >> skipws;
      switch (c) {
      case ' ':  // coordinates
	for (int i = 0; i < 3; ++i) { ss >> coord; _vertices.push_back(coord);}
	break;
      case 'n':  // normal components
	for (int i = 0; i < 3; ++i) { ss >> coord; _normals.push_back(coord);}
	break;
      case 't':  // texture coords.
	if (not  texcoord) {
	  cerr << "Found texture coordinates, which are not yet supported. Ignoring..." << endl;
	  texcoord = true;
	}
	break;
      default:
	cerr << "Seen unknown vertex info of type '" << c << "', ignoring it..." << endl;
	break;  
      }
      break;
      //-------------
    case 'f':  // face info
      ss >> tail;   // tail will contain o d/d/d o d/d o d//d o d:  (same for the rest underneath...)
      first = tail.find("/");
      if (first == string::npos) parseVOnly(ss, tail);
      else {
	second = tail.find("/", first + 1);
	if (second == first + 1)  parseVN(ss, tail);
	else if (second == string::npos) parseVT(ss, tail);
	else parseVTN(ss, tail);
      }
      break;
      //-------------
    case 'm':  // material library
      ss >> tail;
      if (tail != "tllib") {
	cerr << "unknown line of type 'm" << tail << "'. Ignoring..." << endl;
	break;
      }
      ss >> tail;
      loadMTL(modelPath+tail);
      break;
      //-------------
    case 'u':  // material info
      ss >> tail;
      if (tail != "semtl") {
	cerr << "unknown line of type 'u" << tail << "'. Ignoring..." << endl;
	break;
      }
      ss >> tail;
      material = findMat(tail);
      break;
      //-------------
    case 'g':
#if DEBUGPARSER
      cout << "[outer]:Seen line of type '" << c << "', which is not supported. Ignoring it..." << endl;
#endif
      break;
      //-------------
    case 's':
#if DEBUGPARSER
      cout << "[outer]:Seen line of type '" << c << "', which is not supported. Ignoring it..." << endl;
#endif
      break;
      //-------------
    case 'o':
#if DEBUGPARSER
      cout << "[outer]:Seen line of type '" << c << "', which is not supported. Ignoring it..." << endl;
#endif
      break;
      //-------------
    default:
      cout << "[outer]:Seen unknown line of type '" << c << "', ignoring it..." << endl;
      break;
    }
  }
  omplenormals(_faces, _vertices);  // afegim normals per cara...

  // Omplim els vectors per als VBO
  ompleVBOs(_faces, _vertices, _normals, _VBO_vertices, _VBO_normals, 
            _VBO_matamb, _VBO_matdiff, _VBO_matspec, _VBO_matshin);
}

// ======= helper methods for checking and debugging ==========
void Model::dumpStats() const {
  cout << "Model Stats:" << endl;
  cout << "Vertices:   " << _vertices.size() << " components [" << _vertices.size()/3. << " vertices]" << endl;
  cout << "Normals:    " << _normals.size() << " components [" << _normals.size()/3. << " normals]" << endl;
  cout << "Faces:      " << _faces.size() << endl;
}

void Model::dumpModel() const {
  for (unsigned int i = 0; i < _vertices.size(); ++i) {
    if (i%3 == 0) cout << "v ";
    cout << _vertices[i];
    if (i%3 == 2) cout << endl;
    else cout << " ";
  }

  for (unsigned int i = 0; i < _normals.size(); ++i) {
    if (i%3 == 0) cout << "vn ";
    cout << _normals[i];
    if (i%3 == 2) cout << endl;
    else cout << " ";
  }

  for (unsigned int i = 0; i < _faces.size(); ++i) {
    cout << "f";
    if (_faces[i].n.empty()){
      for (int j = 0; j < 3; ++j)
	cout << " " << _faces[i].v[j]/3 + 1;
      cout << endl;
    } else {
      for (int j = 0; j < 3; ++j)
	cout << " " << _faces[i].v[j]/3 + 1 << "//" << _faces[i].n[j]/3 + 1;
      cout << endl;      
    }
  }
}

//======== private methods and auxiliary functions ==========
void Model::parseVOnly(stringstream & ss, string & block) {
#if DEBUGPARSER
  cout << "Entering parseVOnly(..., \""<< block << "\")" << endl;
#endif
  Face f;
  stringstream ssb;
  ssb.str(block);
  int index;
  ssb >> index;
  f.v.push_back(3*index-3);

  ss >> index;
  f.v.push_back(3*index-3);
  
  ss >> index;
  f.v.push_back(3*index-3);
  f.mat = material;
  _faces.push_back(f);
  Face fAnt(f);
  while(ss >> index) {
    f.v.clear();
    f.v.push_back(fAnt.v[0]);
    f.v.push_back(fAnt.v[2]);
    f.v.push_back(3*index-3);
    _faces.push_back(f);
    fAnt = f;
  }
}

void Model::parseVN(stringstream & ss, string & block) {
#if DEBUGPARSER
  cout << "Entering parseVN(..., \""<< block << "\")" << endl;
#endif
  Face f;
  stringstream ssb;
  ssb.str(block);
  int index, n;
  char sep;
  ssb >> index; ssb >> sep; assert(sep == '/'); ssb >> sep; assert(sep == '/');
  ssb >> n;
  f.v.push_back(3*index-3); f.n.push_back(3*n-3);

  ss >> block; 
  ssb.clear(); ssb.str(block);
  ssb >> index; ssb >> sep; assert(sep == '/'); ssb >> sep; assert(sep == '/');
  ssb >> n;
  f.v.push_back(3*index-3); f.n.push_back(3*n-3); 
  
  ss >> block;
  ssb.clear(); ssb.str(block);
  ssb >> index; ssb >> sep; assert(sep == '/'); ssb >> sep; assert(sep == '/');
  ssb >> n;
  f.v.push_back(3*index-3); f.n.push_back(3*n-3);
  f.mat = material;
  _faces.push_back(f);
  Face fAnt(f);
  while(ss >> block) {
    f.v.clear(); f.n.clear();
    ssb.clear(); ssb.str(block);
    ssb >> index; ssb >> sep; assert(sep == '/'); ssb >> sep; assert(sep == '/');
    ssb >> n;
    f.v.push_back(fAnt.v[0]); f.n.push_back(fAnt.n[0]);
    f.v.push_back(fAnt.v[2]); f.n.push_back(fAnt.n[2]);
    f.v.push_back(3*index-3); f.n.push_back(3*n-3);
    _faces.push_back(f);
    fAnt = f;
  }
}

void Model::parseVT(stringstream & ss, string & block) {
#if DEBUGPARSER
  cout << "Entering parseVT(..., \""<< block << "\")" << endl;
#endif
  if (not fvt) {
    cerr << "vt node found: Texture coords not supported yet. Ignoring texture part..." << endl;
    fvt = true;
  }
  Face f;
  stringstream ssb;
  ssb.str(block);
  int index;
  ssb >> index;
  f.v.push_back(3*index-3);

  ss >> block;
  ssb.clear(); ssb.str(block);
  ssb >> index;
  f.v.push_back(3*index-3);
  
  ss >> block;
  ssb.clear(); ssb.str(block);
  ssb >> index;
  f.v.push_back(3*index-3);
  f.mat = material;
  _faces.push_back(f);
  Face fAnt(f);
  while(ss >> block) {
    f.v.clear();
    ssb.clear(); ssb.str(block);
    ssb >> index;
    f.v.push_back(fAnt.v[0]);
    f.v.push_back(fAnt.v[2]);
    f.v.push_back(3*index-3);
    _faces.push_back(f);
    fAnt = f;
  }
}

void Model::parseVTN(stringstream & ss, string & block) {
#if DEBUGPARSER
  cout << "Entering parseVTN(..., \""<< block << "\")" << endl;
#endif
  if (not fvtn) {
    cerr << "vtn node found: Texture coords not supported yet. Ignoring texture part..." << endl;
    fvtn = true;
  }
  Face f;
  stringstream ssb;
  ssb.str(block);
  int index, n, t;
  char sep;
  ssb >> index; ssb >> sep; assert(sep == '/'); ssb >> t >> sep; assert(sep == '/');
  ssb >> n;
  f.v.push_back(3*index-3); f.n.push_back(3*n-3);

  ss >> block;
  ssb.clear(); ssb.str(block);
  ssb >> index; ssb >> sep; assert(sep == '/'); ssb >> t >> sep; assert(sep == '/');
  ssb >> n;
  f.v.push_back(3*index-3); f.n.push_back(3*n-3);
  
  ss >> block;
  ssb.clear(); ssb.str(block);
  ssb >> index; ssb >> sep; assert(sep == '/'); ssb >> t >> sep; assert(sep == '/');
  ssb >> n;
  f.v.push_back(3*index-3); f.n.push_back(3*n-3);
  f.mat = material;
  _faces.push_back(f);
  Face fAnt(f);
  while(ss >> block) {
    f.v.clear(); f.n.clear();
    ssb.clear(); ssb.str(block);
    ssb >> index; ssb >> sep; assert(sep == '/'); ssb >> t >>sep; assert(sep == '/');
    ssb >> n;
    f.v.push_back(fAnt.v[0]); f.n.push_back(fAnt.n[0]);
    f.v.push_back(fAnt.v[2]); f.n.push_back(fAnt.n[2]);
    f.v.push_back(3*index-3); f.n.push_back(3*n-3);
    _faces.push_back(f);
    fAnt = f;
  }
}

static void loadMTL(std::string filename) {
  fstream input(filename.data(), ios::in);
  if (input.rdstate() != ios::goodbit) {
    cerr << "Cannot load MTL file " << filename << endl;
    return;
  }
  string line;
  stringstream ss;
  while (getline(input, line)) {
#if DEBUGPARSER
    cerr << "Just read [" << line << "]" << endl;
#endif
    ss.clear(); ss.str(line);
    string wrd;
    ss >> wrd;
    if (wrd[0] == '#') ;
    else if (wrd == "newmtl") {
#if DEBUGPARSER
    cerr << "Processing '" << wrd << "'" << endl;
#endif
      Material noumat;
      ss >> noumat.name;
      Materials.push_back(noumat);
#if DEBUGPARSER
      Material &current = Materials.back();
      cerr << "Now defining material " << current.name << "(size=" << Materials.size() << ")" <<endl;
#endif
    }
    else if (wrd == "Ns") {
#if DEBUGPARSER
    cerr << "Processing '" << wrd << "'" << endl;
#endif
      ss >> Materials.back().shininess;
    }
    else if (wrd == "Ka") {
#if DEBUGPARSER
    cerr << "Processing '" << wrd << "'" << endl;
#endif
      for (int i = 0; i < 3; ++i) 
	ss >> Materials.back().ambient[i];
    }
    else if (wrd == "Kd") {
#if DEBUGPARSER
    cerr << "Processing '" << wrd << "'" << endl;
#endif
      for (int i = 0; i < 3; ++i) 
	ss >> Materials.back().diffuse[i];
    }
    else if (wrd == "Ks") {
#if DEBUGPARSER
    cerr << "Processing '" << wrd << "'" << endl;
#endif
      for (int i = 0; i < 3; ++i) 
	ss >> Materials.back().specular[i];
    } else {
#if DEBUGPARSER
    cerr << "MTL parser: read line of type " << wrd << " which is not supported. Skipped..." << endl;
#endif
    }
  }
}

static int findMat(string material) {
  for (unsigned int i = 0; i < Materials.size(); ++i) 
    if (Materials[i].name == material) return i;
  return 0;
}


static void omplenormals(vector<Face> &_faces, 
			 const vector<Vertex>  &_vertices) {
  for (unsigned int i = 0; i < _faces.size(); ++i) {
    double v0[3], v1[3];
    int P0 =_faces[i].v[0];
    int P1 =_faces[i].v[1];
    int P2 =_faces[i].v[2];
    for (int j = 0; j < 3; ++j) {
      v0[j] = _vertices[P1+j] - _vertices[P0+j];
      v1[j] = _vertices[P2+j] - _vertices[P1+j];
    }
    double norm = 0;
    double *normalcara = _faces[i].normalC;
    normalcara[0] = v0[1]*v1[2] - v0[2]*v1[1]; 
    norm += normalcara[0]*normalcara[0];
    normalcara[1] = v0[2]*v1[0] - v0[0]*v1[2]; 
    norm += normalcara[1]*normalcara[1];
    normalcara[2] = v0[0]*v1[1] - v0[1]*v1[0]; 
    norm += normalcara[2]*normalcara[2];
    for (int j = 0; j < 3; ++j) normalcara[j] /= sqrt(norm);
  }
}

static void ompleVBOs(vector<Face> &_faces, 
		      const vector<Vertex> &_vertices,
                      const vector<Normal> &_normals,
		      float *&_VBO_vert, float *&_VBO_norm, 
                      float *&_VBO_mata, float *&_VBO_matd, float *&_VBO_matsp, float *&_VBO_matsh) 
{
  // Creem els VBOs amb 3*3*faces.size() doubles cadascun
  _VBO_vert = new float[3*3*_faces.size()];  
  _VBO_norm = new float[3*3*_faces.size()];
  _VBO_mata = new float[3*3*_faces.size()];
  _VBO_matd = new float[3*3*_faces.size()];
  _VBO_matsp = new float[3*3*_faces.size()];
  _VBO_matsh = new float[3*_faces.size()];

  int index = 0;
  for (unsigned int f = 0; f < _faces.size(); ++f) {
    Material &mat = Materials[_faces[f].mat];
    for (int i = 0; i < 3; ++i) {
      int P =_faces[f].v[i];
      for (int j = 0; j < 3; ++j) {
        _VBO_vert[index+j] = _vertices[P+j];
	if (_normals.size() != 0) {
          _VBO_norm[index+j] = _normals[_faces[f].n[i]+j];
        }
	else {
          _VBO_norm[index+j] = _faces[f].normalC[j];
        }	
        _VBO_mata[index+j] = mat.ambient[j];
        _VBO_matd[index+j] = mat.diffuse[j];
        _VBO_matsp[index+j] = mat.specular[j];
      }
      _VBO_matsh[index/3] = mat.shininess;
      index += 3;
    }
  }
}
