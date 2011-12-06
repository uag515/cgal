#ifndef CGAL_VOLUME_PLANE_H
#define CGAL_VOLUME_PLANE_H

#include <GL/glew.h>
#include <CGAL/gl.h>
#include <CGAL_demo/Scene_item.h>

#include <vector>
#include <cassert>

#include <QDebug>
#include <QGLViewer/qglviewer.h>

#include "Volume_plane_interface.h"

// #if !defined(NDEBUG)
// void printGlError(unsigned int line) {
//   GLenum error = glGetError();
//   if(error != GL_NO_ERROR)
//     qDebug() << gluErrorString(error) << "@" << line;
// }
// #else
// void printGlError(unsigned int) {
// }
// #endif

struct XSel { double& operator()(qglviewer::Vec& v) const { return v.x; } };
struct YSel { double& operator()(qglviewer::Vec& v) const { return v.y; } };
struct ZSel { double& operator()(qglviewer::Vec& v) const { return v.z; } };

template<typename DimSelector>
class Length_constraint : public qglviewer::WorldConstraint {
public:
  Length_constraint(double max) : max(max) { }

  void constrainTranslation(qglviewer::Vec& t, qglviewer::Frame* frame) {
    WorldConstraint::constrainTranslation(t, frame);
    qglviewer::Vec pos = frame->position();
    double start = DimSelector()(pos);
    double end = DimSelector()(t);
    start += end;

    if(start > max || (start < 0)) {
      DimSelector()(t) = 0.0;
    }
  }

private:
  double max;
};

struct x_tag {};
struct y_tag {};
struct z_tag {};

template<typename Tag>
class Volume_plane : public Volume_plane_interface, public Tag {
public:
  Volume_plane(unsigned int adim, unsigned int bdim, unsigned int cdim, 
               float xscale, float yscale, float zscale, std::vector<float>& colors);

  virtual ~Volume_plane();

  Volume_plane* clone() const { return NULL; }

  virtual RenderingMode renderingMode() const { return Flat; }
  bool supportsRenderingMode(RenderingMode m) const { return m == Flat; }
  
  QString toolTip() const { return "Plane through a volume"; }
  QString name() const { return name(*this); }

  bool isFinite() const { return true; }
  bool isEmpty() const { return false; }
  bool manipulatable() const { return true; }
  ManipulatedFrame* manipulatedFrame() { return mFrame_; }

  unsigned int cube() {return currentCube; }

  void draw() const;

  unsigned int aDim() const { return adim_; }
  unsigned int bDim() const { return bdim_; }
  unsigned int cDim() const { return cdim_; }

  qglviewer::Vec translationVector() const {
    return translationVector(*this);
  }

  // uses a public init function to make enable construction in
  // threads without gl-context
  void init();

private:
  static const char* vertexShader; 

  static const char* fragmentShader;

  qglviewer::Vec translationVector(x_tag) const {
    return qglviewer::Vec(xscale_, 0.0, 0.0);
  }
  qglviewer::Vec translationVector(y_tag) const {
    return qglviewer::Vec(0.0, yscale_, 0.0);
  }
  qglviewer::Vec translationVector(z_tag) const {
    return qglviewer::Vec(0.0, 0.0, zscale_);
  }

  void initShaders();

  void buildVertex(std::vector<float>& out, unsigned int i, unsigned int j) {
    buildVertex(out, i, j, *this);
  }

  void buildVertex(std::vector<float>& out, unsigned int i, unsigned int j, x_tag) {
    out.push_back(0.0f);
    out.push_back(i * yscale_);
    out.push_back(j * zscale_);
  }

  void buildVertex(std::vector<float>& out, unsigned int i, unsigned int j, y_tag) {
    out.push_back(i * xscale_);
    out.push_back(0.0f);
    out.push_back(j * zscale_);
  }

  void buildVertex(std::vector<float>& out, unsigned int i, unsigned int j, z_tag) {
    out.push_back(i * xscale_);
    out.push_back(j * yscale_);
    out.push_back(0.0f);
  }

  const unsigned int adim_, bdim_, cdim_;
  const double xscale_, yscale_, zscale_;
  mutable int currentCube;
  ManipulatedFrame* mFrame_;

  GLuint vVBO;
  GLuint program;
  std::vector< std::pair<GLuint, unsigned int> > ebos;
  std::vector< float > colors_;

  QString name(x_tag) const { return tr("X Slice for %1").arg(name_); }
  QString name(y_tag) const { return tr("Y Slice for %2").arg(name_); }
  QString name(z_tag) const { return tr("Z Slice for %2").arg(name_); }

  void drawRectangle(x_tag) const {
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3f(0.0f, (adim_ - 1) * yscale_, 0.0f);
      glVertex3f(0.0f, (adim_ - 1) * yscale_, (bdim_ - 1) * zscale_);
      glVertex3f(0.0f, 0.0f, (bdim_ - 1) * zscale_);
  }

  void drawRectangle(y_tag) const {
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3f((adim_ - 1) * xscale_, 0.0f, 0.0f);
      glVertex3f((adim_ - 1) * xscale_, 0.0f, (bdim_ - 1) * zscale_);
      glVertex3f(0.0f, 0.0f, (bdim_ - 1) * zscale_);
  }

  void drawRectangle(z_tag) const {
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3f((adim_ - 1) * xscale_, 0.0f, 0.0f);
      glVertex3f((adim_ - 1) * xscale_, (bdim_ - 1) * yscale_, 0.0f);
      glVertex3f(0.0f, (bdim_ - 1) * yscale_, 0.0f);
  }

  qglviewer::Constraint* setConstraint(x_tag) {
    qglviewer::AxisPlaneConstraint* c = new Length_constraint<XSel>(cdim_ * xscale_);
    c->setRotationConstraintType(qglviewer::AxisPlaneConstraint::FORBIDDEN);
    c->setTranslationConstraint(qglviewer::AxisPlaneConstraint::AXIS, qglviewer::Vec(1.0f, 0.0f, 0.0f));
    return c;
  }
  
  qglviewer::Constraint* setConstraint(y_tag) {
    qglviewer::AxisPlaneConstraint* c = new Length_constraint<YSel>(cdim_ * yscale_);
    c->setRotationConstraintType(qglviewer::AxisPlaneConstraint::FORBIDDEN);
    c->setTranslationConstraint(qglviewer::AxisPlaneConstraint::AXIS, qglviewer::Vec(0.0f, 1.0f, 0.0f));
    return c;
  }

  qglviewer::Constraint* setConstraint(z_tag) {
    qglviewer::AxisPlaneConstraint* c = new Length_constraint<ZSel>(cdim_ * zscale_);
    c->setRotationConstraintType(qglviewer::AxisPlaneConstraint::FORBIDDEN);
    c->setTranslationConstraint(qglviewer::AxisPlaneConstraint::AXIS, qglviewer::Vec(0.0f, 0.0f, 1.0f));
    return c;
  }

  void updateCurrentCube() const { currentCube = getTranslation(); }

  GLdouble getTranslation() const { return getTranslation(*this); }
  GLdouble getTranslation(x_tag) const { return mFrame_->matrix()[12] / xscale_; }
  GLdouble getTranslation(y_tag) const { return mFrame_->matrix()[13] / yscale_; }
  GLdouble getTranslation(z_tag) const { return mFrame_->matrix()[14] / zscale_; }

};

template<typename T>
const char* Volume_plane<T>::vertexShader = 
      "#version 330 \n"
      "layout(location = 0) in vec3 position; \n"
      "layout(location = 1) in float color; \n"
      "uniform mat4 modelviewMatrix; \n"
      "uniform mat4 projectionMatrix; \n"
      "out vec4 fullColor; \n"
      "void main() \n"
      "{ gl_Position = (projectionMatrix * modelviewMatrix) * vec4(position.x, position.y, position.z, 1.0); \n"
      "  fullColor = vec4(color, color, color, 1.0f); } \n";

template<typename T>
const char* Volume_plane<T>::fragmentShader = 
      "#version 330 \n"
      "smooth in vec4 fullColor; \n"
      "out vec4 outputColor; \n"
      "void main() { outputColor = fullColor; } \n";


template<typename T>
Volume_plane<T>::Volume_plane(unsigned int adim, unsigned int bdim, unsigned int cdim, 
                              float xscale, float yscale, float zscale, std::vector<float>& colors)
  : adim_(adim), bdim_(bdim), cdim_(cdim), xscale_(xscale), 
    yscale_(yscale), zscale_(zscale), currentCube(0),
    mFrame_(new qglviewer::ManipulatedFrame) {
  colors_.swap(colors);
    
  mFrame_->setConstraint(setConstraint(*this));
}

template<typename T>
Volume_plane<T>::~Volume_plane() {
  for(std::vector< std::pair< GLuint, unsigned int> >::const_iterator it = ebos.begin(); 
      it != ebos.end(); ++it) { 
    glDeleteBuffers(1, &(it->first));
  }
  glDeleteProgram(program);
  delete mFrame_;
}

template<typename T>
void Volume_plane<T>::draw() const {
  updateCurrentCube();

  glDisable(GL_LIGHTING);
  glPushMatrix();
  glMultMatrixd(mFrame_->matrix());

  GLint renderMode;
  glGetIntegerv(GL_RENDER_MODE, &renderMode);
  if(renderMode == GL_SELECT) {
    // draw a quick bounding box
    glBegin(GL_QUADS);
    drawRectangle(*this);
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
    return;
  }

  glLineWidth(3.0f);
  glBegin(GL_LINE_LOOP);
  drawRectangle(*this);
  glEnd();
  glLineWidth(1.0f);

  glUseProgram(program);
  // printGlError(__LINE__);

  float matrices[32];
  glGetFloatv(GL_MODELVIEW_MATRIX, &matrices[0]);
  glGetFloatv(GL_PROJECTION_MATRIX, &matrices[16]);
  glUniformMatrix4fv(glGetUniformLocation(program, "modelviewMatrix"), 1, GL_FALSE, &matrices[0]);
  glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_FALSE, &matrices[16]);

  glBindBuffer(GL_ARRAY_BUFFER, vVBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, &(colors_[ currentCube * adim_ * bdim_ ]));
  glEnableVertexAttribArray(1);

  // printGlError(__LINE__);

  for(unsigned int i = 0; i < ebos.size(); ++i)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos[i].first);
    glDrawElements(GL_QUADS, ebos[i].second, GL_UNSIGNED_INT, 0);
  }

  // printGlError(__LINE__);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glPopMatrix();

  glUseProgram(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glEnable(GL_LIGHTING);

  // printGlError(__LINE__);
}

template<typename T>
void Volume_plane<T>::init() {
  
  initShaders();

  // for each vertex
  std::vector< float > vertices;
  vertices.reserve(bdim_ * adim_ * 3);
  for(unsigned int i = 0; i < adim_; ++i) 
  {
    for(unsigned int j = 0; j < bdim_; ++j)
    {
      buildVertex(vertices, i, j);
    }
  }
    
  std::cout << "vertex size: " << vertices.size() << std::endl;
  assert(vertices.size() == (3 * adim_ * bdim_));

  int maxi, maxv;
  glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxi);
  glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxv);
  assert((vertices.size( ) / 3) < (unsigned int)maxi);

  glGenBuffers(1, &vVBO);
  glBindBuffer(GL_ARRAY_BUFFER, vVBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // printGlError(__LINE__);

  // for each patch
  std::vector<unsigned int> indices;
  for(unsigned int j = 0; j < adim_ - 1; ++j) {
    for(unsigned int k = 0; k < bdim_ - 1; ++k) {
      indices.push_back( j * bdim_ + k );
      assert(indices.back() < (vertices.size() / 3));
    
      indices.push_back( j * bdim_ + (k + 1) );
      assert(indices.back() < (vertices.size() / 3));
    
      indices.push_back( (j+1) * bdim_ + (k+1) );
      assert(indices.back() < (vertices.size() / 3));
    
      indices.push_back( (j+1) * bdim_ + (k) );
      assert(indices.back() < (vertices.size() / 3));
    }
  }

  assert((indices.size() / 4) == (adim_ - 1) * (bdim_ - 1));

  const unsigned int slice = 64000;
  for(unsigned int i = 0; i < indices.size(); i+=slice)
  {
    GLuint ebo;
    unsigned int left_over = (i + slice) > indices.size()  ? std::distance(indices.begin() + i, indices.end()) : slice;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * left_over, &indices[i], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    ebos.push_back(std::make_pair(ebo, left_over));
  }

  // printGlError(__LINE__);
}

template<typename T>
void Volume_plane<T>::initShaders() {
  program = glCreateProgram();
  GLint status;
  char buffer[1024];
  int len;

  GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
  GLint* lengths = { 0 };
  glShaderSource(vertex, 1, &vertexShader, lengths);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &status);

  glGetShaderInfoLog(vertex, 1024, &len, buffer);
  std::cout << "VertexShader: " << buffer << std::endl;
  assert(status == GL_TRUE);

  // printGlError(__LINE__);

  GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentShader, lengths);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &status);

  glGetShaderInfoLog(fragment, 1024, &len, buffer);
  std::cout << "FragmentShader: " << buffer << std::endl;

  assert(status == GL_TRUE);

  // printGlError(__LINE__);

  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  assert(status == GL_TRUE);

  // printGlError(__LINE__);
}



#endif /* CGAL_VOLUME_PLANE_H */
