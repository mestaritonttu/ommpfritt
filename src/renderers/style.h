#pragma once

#include "aspects/propertyowner.h"
#include "aspects/selectable.h"
#include "color/color.h"
#include "aspects/copycreatable.h"
#include "aspects/listelement.h"

namespace omm
{

class Scene;

class Style
  : public PropertyOwner<AbstractPropertyOwner::Kind::Style>
  , public Selectable
  , public virtual Serializable
  , public Copyable<Style>
{
public:
  explicit Style(Scene* scene = nullptr);
  std::unique_ptr<Style> copy() const override;

private:
  Scene* const m_scene;

public:
  static constexpr auto PEN_IS_ACTIVE_KEY = "pen/active";
  static constexpr auto PEN_COLOR_KEY = "pen/color";
  static constexpr auto PEN_WIDTH_KEY = "pen/width";
  static constexpr auto BRUSH_IS_ACTIVE_KEY = "brush/active";
  static constexpr auto BRUSH_COLOR_KEY = "brush/color";
};

}  // namespace omm
