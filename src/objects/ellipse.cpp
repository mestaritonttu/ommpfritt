#include "objects/ellipse.h"
#include <QObject>
#include "properties/floatproperty.h"
#include "objects/path.h"
#include "properties/vectorproperty.h"
#include "properties/integerproperty.h"

namespace omm
{

class Style;

Ellipse::Ellipse(Scene* scene) : AbstractProceduralPath(scene)
{
  static const auto category = QObject::tr("ellipse").toStdString();
  add_property<FloatVectorProperty>(RADIUS_PROPERTY_KEY, arma::vec2 { 100.0, 100.0 } )
    .set_label(QObject::tr("r").toStdString()).set_category(category);
  add_property<IntegerProperty>(CORNER_COUNT_PROPERTY_KEY, 100)
    .set_range(3, IntegerPropertyLimits::upper)
    .set_label(QObject::tr("n").toStdString()).set_category(category);
}

BoundingBox Ellipse::bounding_box()
{
  const auto r = property(RADIUS_PROPERTY_KEY).value<VectorPropertyValueType<arma::vec2>>();
  return BoundingBox({ -r, r });
}

std::string Ellipse::type() const
{
  return TYPE;
}

std::unique_ptr<Object> Ellipse::clone() const
{
  return std::make_unique<Ellipse>(*this);
}

std::vector<Point> Ellipse::points()
{
  const size_t n = std::max(3, property(CORNER_COUNT_PROPERTY_KEY).value<int>());
  const auto r = property(RADIUS_PROPERTY_KEY).value<VectorPropertyValueType<arma::vec2>>();

  std::vector<Point> points;
  points.reserve(n);
  for (size_t i = 0; i < n; ++i) {
    const double x = cos(i * 2.0/n * M_PI) * r(0);
    const double y = sin(i * 2.0/n * M_PI) * r(1);
    points.push_back(Point(arma::vec2 { x, y }));
  }
  return points;
}

bool Ellipse::is_closed() const
{
  return true;
}

AbstractPropertyOwner::Flag Ellipse::flags() const
{
  return Object::flags() | Flag::Convertable | Flag::IsPathLike;
}


}  // namespace omm
