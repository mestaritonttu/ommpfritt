#pragma once

#include <iterator>
#include <array>
#include <vector>
#include <QColor>
#include <map>

namespace omm
{

/**
 * @brief The Color class r, g, b, h, s, v are in range [0, 1]
 */
class Color
{
private:
  std::array<double, 4> m_components;

public:
  enum Model { RGBA, HSVA, Named };
  Color(Model model, const std::array<double, 3> components, const double alpha);
  Color(Model model, const std::array<double, 4> components);
  explicit Color(const QString& name);
  explicit Color(const QColor& c);
  explicit Color();

  QString to_html() const;
  static Color from_html(const QString& html, bool *ok = nullptr);
  static Color from_qcolor(const QColor& color);
  QColor to_qcolor() const;

  void to_ordinary_color();
  QString name() const;

  enum class Role { Red, Green, Blue, Hue, Saturation, Value, Alpha };

  void set(Role role, double value);
  double get(Role role) const;

  static std::array<double, 4> convert(Model from, Model to, const std::array<double, 4> values);
  void convert(Model to);
  Color convert(Model to) const;

  std::array<double, 4> components(Model model) const;
  void set_components(Model model, const std::array<double, 4>& components);
  Model model() const { return m_current_model; }

  static const std::map<Model, std::array<QString, 4>> component_names;

private:
  Model m_current_model;
  QString m_name = "";

  friend bool operator==(const Color& a, const Color& b);
  friend bool operator<(const Color& a, const Color& b);

  /**
   * @brief component returns a reference to the component represented by a certain role.
   * @param role the role. Note that the role must belong to the underlying model.
   * @return a reference to the component.
   */
  double& component(Role role);

  static Model model(Role role, Model tie);
};

namespace Colors
{

static const Color RED      (Color::Model::RGBA, { 1.0, 0.0, 0.0, 1.0 });
static const Color GREEN    (Color::Model::RGBA, { 0.0, 1.0, 0.0, 1.0 });
static const Color BLUE     (Color::Model::RGBA, { 0.0, 0.0, 1.0, 1.0 });
static const Color YELLOW   (Color::Model::RGBA, { 1.0, 1.0, 0.0, 1.0 });
static const Color BLACK    (Color::Model::RGBA, { 0.0, 0.0, 0.0, 1.0 });
static const Color WHITE    (Color::Model::RGBA, { 1.0, 1.0, 1.0, 1.0 });
static const Color CERULEAN (Color::Model::RGBA, { 0.1, 0.52, 0.82, 1.0 });

}

bool operator==(const Color& a, const Color& b);
bool operator!=(const Color& a, const Color& b);
bool operator<(const Color& a, const Color& b);
std::ostream& operator<<(std::ostream& ostream, const Color& color);


}  // namespace
