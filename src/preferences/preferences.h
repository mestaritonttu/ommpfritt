#pragma once

#include <map>
#include <QString>
#include <QObject>
#include <Qt>
#include <QColor>

namespace omm
{

class Preferences : public QObject
{
  Q_OBJECT
public:
  Preferences();
  ~Preferences();

  struct MouseModifier
  {
    MouseModifier(const QString& label,
                  Qt::MouseButton default_button, Qt::KeyboardModifiers default_modifiers);
    const QString label;
    Qt::MouseButton button;
    Qt::KeyboardModifiers modifiers;
  };

  struct GridOption
  {
    enum class ZOrder { Invisible = 0, Foreground = 1, Background = 2 };
    GridOption(const QString& label, const Qt::PenStyle& pen_style, double pen_width, double base);
    const QString label;
    Qt::PenStyle pen_style;
    double pen_width;
    double base;
    ZOrder zorder = ZOrder::Background;
  };

  std::map<QString, MouseModifier> mouse_modifiers;
  std::map<QString, GridOption> grid_options;

};

}  // namespace omm
