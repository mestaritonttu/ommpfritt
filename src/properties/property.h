#pragma once

#include <string>
#include <typeinfo>
#include <set>
#include <variant>
#include <functional>

#include "logging.h"
#include "external/json.hpp"
#include "abstractfactory.h"
#include "aspects/serializable.h"
#include "common.h"
#include "color/color.h"
#include "geometry/vec2.h"
#include "variant.h"
#include "animation/track.h"

namespace omm
{

class Object;
class Track;
class Property;
class AbstractPropertyOwner;
class OptionsProperty;

class Property
  : public QObject
  , public AbstractFactory<std::string, Property>
  , public virtual Serializable
{
  Q_OBJECT
public:
  struct Configuration : std::map<std::string, std::variant<bool, int, double, Vec2i, Vec2f,
                                                            std::size_t, std::string,
                                                            std::vector<std::string>>>
  {
    using variant_type = value_type::second_type;
    template<typename T> T get(const std::string& key) const
    {
      if constexpr (std::is_enum_v<T>) {
        return static_cast<T>(get<std::size_t>(key));
      } else {
        const auto cit = find(key);
        assert (cit != end());

        const T* value = std::get_if<T>(&cit->second);
        assert(value != nullptr);
        return *value;
      }
    }

    template<typename T> T get(const std::string& key, const T& default_value) const
    {
      if constexpr (std::is_enum_v<T>) {
        return static_cast<T>(get<std::size_t>(key, default_value));
      } else {
        const auto cit = find(key);
        if (cit == end()) {
          return default_value;
        } else {
          const T* value = std::get_if<T>(&cit->second);
          if (value != nullptr) {
            return *value;
          } else {
            return default_value;
          }
        }
      }
    }
  };

  Property() = default;
  explicit Property(const Property& other);
  virtual ~Property() = default;

  static constexpr auto LABEL_POINTER = "label";
  static constexpr auto CATEGORY_POINTER = "category";
  static constexpr auto ANIMATABLE_POINTER = "animatable";
  static constexpr auto TRACK_POINTER = "track";
  static constexpr auto IS_ANIMATED_POINTER = "animated";

  virtual std::string widget_type() const;

  template<typename ResultT, typename PropertyT, typename MemFunc> static
  ResultT get_value(const std::set<Property*>& properties, MemFunc&& f)
  {
    const auto values = ::transform<ResultT>(properties, [&f](const Property* property) {
      return f(static_cast<const PropertyT&>(*property));
    });

    if (values.size() > 1) {
      LWARNING << "expected uniform value, but got " << values.size() << " different values.";
    }

    return *values.begin();
  }

  template<typename ResultT, typename MemFunc> static ResultT
  get_value(const std::set<Property*>& properties, MemFunc&& f)
  {
    return Property::get_value<ResultT, Property, MemFunc>(properties, std::forward<MemFunc>(f));
  }

  virtual bool is_compatible(const Property& other) const;
  static constexpr auto USER_PROPERTY_CATEGROY_NAME = QT_TRANSLATE_NOOP("Property",
                                                                        "user properties");

  // user properties can be added/edited/removed dynamically
  bool is_user_property() const;

  virtual std::unique_ptr<Property> clone() const = 0;

  virtual void revise();

  // === set/get value
public:
  virtual variant_type variant_value() const = 0;
  virtual void set(const variant_type& value) = 0;
  template<typename EnumT> std::enable_if_t<std::is_enum_v<EnumT>, void>
  set(const EnumT& value) { set(static_cast<std::size_t>(value)); }
  template<typename ValueT> std::enable_if_t<!std::is_enum_v<ValueT>, ValueT>
  value() const { return std::get<ValueT>(variant_value()); }
  template<typename ValueT> std::enable_if_t<std::is_enum_v<ValueT>, ValueT>
  value() const { return static_cast<ValueT>(std::get<std::size_t>(variant_value())); }

  // === Configuration ====
public:
  bool is_visible() const;
  void set_visible(bool visible);
  std::string label() const;
  std::string category() const;
  bool is_animatable() const;
  Property& set_label(const std::string& label);
  Property& set_category(const std::string& category);
  Property& set_animatable(bool animatable);
  Configuration configuration;
private:
  bool m_is_visible = true;

  // === (De)Serialization
public:
  void serialize(AbstractSerializer& serializer, const Serializable::Pointer& root) const override;
  void deserialize(AbstractDeserializer& deserializer, const Serializable::Pointer& root) override;

  // === Animation
public:
  Track* track() const;
  void set_track(std::unique_ptr<Track> track);
  std::unique_ptr<Track> extract_track();
private:
  std::unique_ptr<Track> m_track;

Q_SIGNALS:
  void value_changed(Property*);
  void visibility_changed(bool);

};

void register_properties();

}  // namespace ommAbstractPropertyOwner
