#pragma once

#include <QWidget>
#include "geometry/point.h"

class QPushButton;

namespace omm
{

class CoordinateEdit;
class Point;
class Path;

class PointEdit : public QWidget
{
  Q_OBJECT
public:
  PointEdit(Point& point, Path* path, QWidget* parent = nullptr);
private:
  QPushButton* m_mirror_from_left;
  QPushButton* m_mirror_from_right;
  QPushButton* m_coupled;
  QPushButton* m_vanish_left;
  QPushButton* m_vanish_right;
  CoordinateEdit* m_left_tangent_edit;
  CoordinateEdit* m_right_tangent_edit;
  CoordinateEdit* m_position_edit;
  Point& m_point;
  Path* m_path;

private Q_SLOTS:
  void mirror_from_right();
  void mirror_from_left();
  void set_left_maybe(const PolarCoordinates& old_right, const PolarCoordinates& new_right);
  void set_right_maybe(const PolarCoordinates& old_left, const PolarCoordinates& new_left);
  void update_point();
};

}  // namespace omm
