#pragma once

#include "serializers/abstractserializer.h"
#include "properties/property.h"

namespace omm
{

namespace TypedPropertyDetail
{

static constexpr auto VALUE_POINTER = "value";
static constexpr auto DEFAULT_VALUE_POINTER = "default_value";

} // namespace TypedPropertyDetail

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @tparam ValueT ValueT must provide
 *  bool operator==(const ValueT&, const ValueT&)
 *  bool operator!=(const ValueT&, const ValueT&)
 *  bool operator<(const ValueT&, const ValueT&)
 *  or equivalent non-member functions.
 */
template<typename ValueT>
class TypedProperty : public Property
{
public:
  using value_type = ValueT;
  TypedProperty(ValueT defaultValue = ValueT())
    : m_value(defaultValue), m_default_value(defaultValue) {}

public:
  variant_type variant_value() const override { return m_value; }
  ValueT value() const { return Property::value<ValueT>(); }
  void set(const variant_type& variant) override { set(std::get<ValueT>(variant)); }

  virtual void set(const ValueT& value)
  {
    if (m_value != value) {
      m_value = value;
      Q_EMIT value_changed(this);
    }
  }

  virtual ValueT default_value() const { return m_default_value; }
  virtual void set_default_value(const ValueT& value) { m_default_value = value; }
  virtual void reset() { m_value = m_default_value; }
  QString type() const override = 0;

  bool is_numerical() const override
  {
    return std::is_same_v<bool, ValueT> || std::is_same_v<double, ValueT>
        || std::is_same_v<int, ValueT> || std::is_same_v<Vec2f, ValueT>
        || std::is_same_v<Vec2i, ValueT>;
  }

private:
  ValueT m_value;
  ValueT m_default_value;
};

template<typename ValueT> QString TypedProperty<ValueT>::type() const
{
  // TypedProperty<ValueT> has no meaningful way to provide type(), which is realised
  // by overriding the virtual type() member in any of the derived types.
  LFATAL("It is tempting to call me. However, I'm useless. Don't call me.");
  return "";
}

}  // namespace
