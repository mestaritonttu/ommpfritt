#pragma once

#include <glog/logging.h>
#include <assert.h>
#include <map>
#include <memory>
#include <typeinfo>
#include "properties/property.h"
#include "orderedmap.h"
#include "external/json_fwd.hpp"
#include "aspects/serializable.h"
#include "properties/typedproperty.h"

namespace omm
{

using PropertyMap = OrderedMap<std::string, std::unique_ptr<Property>>;

class AbstractPropertyOwner : public virtual Serializable, public AbstractPropertyObserver
{
public:
  enum class Kind { Tag = 0x1, Style = 0x2, Object = 0x4 };
  explicit AbstractPropertyOwner();
  AbstractPropertyOwner(AbstractPropertyOwner&& other);
  virtual ~AbstractPropertyOwner();
  using Key = PropertyMap::key_type;
  Property& property(const Key& key) const;

  template<typename PropertyT> PropertyT& property(const Key& key) const
  {
    assert(has_property<PropertyT>(key));
    return *this->property(key).cast<PropertyT>();
  }

  bool has_property(const Key& key) const;

  template<typename PropertyT> bool has_property(const Key& key) const
  {
    return this->has_property(key) && this->property(key).is_type<PropertyT>();
  }

  const PropertyMap& properties() const;

  void serialize(AbstractSerializer& serializer, const Pointer& root) const override;
  void deserialize(AbstractDeserializer& deserializer, const Pointer& root) override;
  virtual std::string name() const = 0;
  virtual void on_property_value_changed() override;
  virtual Kind kind() const = 0;

protected:
  Property& add_property(const Key& key, std::unique_ptr<Property> property);

private:
  PropertyMap m_properties;
};

template<AbstractPropertyOwner::Kind kind_> class PropertyOwner : public AbstractPropertyOwner
{
public:
  using AbstractPropertyOwner::AbstractPropertyOwner;
  static constexpr Kind KIND = kind_;
  Kind kind() const override { return KIND; }
};

}  // namespace omm

omm::AbstractPropertyOwner::Kind
operator|(omm::AbstractPropertyOwner::Kind a, omm::AbstractPropertyOwner::Kind b);
omm::AbstractPropertyOwner::Kind
operator&(omm::AbstractPropertyOwner::Kind a, omm::AbstractPropertyOwner::Kind b);
bool operator!(omm::AbstractPropertyOwner::Kind a);
