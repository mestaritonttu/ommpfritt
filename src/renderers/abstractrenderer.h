#pragma once

#include <string>
#include <stack>

#include "geometry/objecttransformation.h"
#include "geometry/boundingbox.h"
#include "geometry/point.h"

namespace omm
{

class Style;
class Scene;

class AbstractRenderer
{
public:
  explicit AbstractRenderer(Scene& scene);
  void render();
  const BoundingBox& bounding_box() const;

public:
  void set_base_transformation(const ObjectTransformation& base_transformation);
  virtual void start_group(const std::string& name) {}
  virtual void end_group() {}
  virtual void draw_spline( const std::vector<Point>& points, const Style& style,
                            bool closed = false ) = 0;
  virtual void draw_rectangle(const arma::vec2& pos, const double radius, const Style& style) = 0;
  virtual void push_transformation(const ObjectTransformation& transformation);
  virtual void pop_transformation();
  virtual ObjectTransformation current_transformation() const;
  virtual void draw_circle(const arma::vec2& pos, const double radius, const Style& style) = 0;
  virtual
  void draw_image(const std::string& filename, const arma::vec2& pos, const arma::vec2& size) = 0;
  Scene& scene;

private:
  ObjectTransformation m_base_transformation;
  std::stack<ObjectTransformation> m_transformation_stack;
};

}  // namespace omm
