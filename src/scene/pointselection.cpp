#include "scene/pointselection.h"
#include "objects/path.h"
#include "scene/scene.h"

namespace omm
{

PointSelection::PointSelection(Scene &scene) : m_scene(scene) {}

std::set<Point> PointSelection::points(bool skip_root) const
{
  std::set<Point> selected_points;
  for (auto* path : type_cast<Path*>(m_scene.item_selection<Object>())) {
    for (auto* point : path->points_ref()) {
      if (point->is_selected) {
        selected_points.insert(path->global_transformation(skip_root).apply(*point));
      }
    }
  }
  return selected_points;
}

Vec2f PointSelection::center(bool skip_root) const
{
  const auto selected_points = points(skip_root);
  Vec2f sum(0.0, 0.0);
  for (const Point& p : selected_points) {
    sum += p.position;
  }
  return sum / static_cast<double>(selected_points.size());
}

}  // namespace omm
