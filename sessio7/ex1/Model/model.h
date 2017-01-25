/*
 *  model.h
 *  (c) 2012 ViRVIG (http://www.virvig.eu)
 *  This source code is licensed under the creative commons CC BY-NC-SA 3.0
 *  license (see http://creativecommons.org/licenses/by-nc-sa/3.0/)
 *
 */

#ifndef MODEL_H
#define MODEL_H    

#include <vector>
#include <string>

struct Material {
  std::string name;
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;
  Material();
};
#ifndef __MODEL__DEF__ 
extern
#endif
std::vector<Material> Materials
#ifdef __MODEL__DEF__
(1); 
#else
;
#endif

typedef double Vertex;
typedef double Normal;

struct Face{
  std::vector<int> v;   // Model::load() only generates triangles, though.
  std::vector<int> n;
  int mat;
  double normalC[3];
};

class Model {
 public:
  Model();
  ~Model();
  void load(std::string filename);
  const std::vector<Vertex>& vertices() const {
    return _vertices;
  }
  const std::vector<Normal>& normals() const {
    return _normals;
  }
  const std::vector<Face>& faces() const {
    return _faces;
  }
  void dumpStats() const;
  void dumpModel() const;

  float *VBO_vertices () {
    return _VBO_vertices;
  }
  float *VBO_normals () {
    return _VBO_normals;
  }
  float *VBO_matamb () {
    return _VBO_matamb;
  }
  float *VBO_matdiff () {
    return _VBO_matdiff;
  }
  float *VBO_matspec () {
    return _VBO_matspec;
  }
  float *VBO_matshin () {
    return _VBO_matshin;
  }

 private:
  std::vector<Vertex> _vertices;
  std::vector<Normal> _normals;
  std::vector<Face> _faces;

  float *_VBO_vertices, *_VBO_normals;
  float *_VBO_matamb, *_VBO_matdiff, *_VBO_matspec, *_VBO_matshin;

  void parseVOnly(std::stringstream & ss, std::string & block);
  void parseVN(std::stringstream & ss, std::string & block);
  void parseVT(std::stringstream & ss, std::string & block);
  void parseVTN(std::stringstream & ss, std::string & block);
};

#endif // MODEL_H
