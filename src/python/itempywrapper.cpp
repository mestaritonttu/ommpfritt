#include "python/itempywrapper.h"
#include <pybind11/stl.h>
#include "tags/tag.h"
#include "renderers/style.h"
#include "objects/object.h"

namespace omm
{


// -- Tag ---

PyWrapper<Tag>::PyWrapper(Tag& tag)
  : PropertyOwnerPyWrapper(tag)
  , m_wrapped(&tag)
{
}

py::object PyWrapper<Tag>::get_owner() const
{
  return py::cast(PyWrapper<Object>(*m_wrapped->owner()));
}

// -- Object ---

PyWrapper<Object>::PyWrapper(Object& object)
  : PropertyOwnerPyWrapper(object)
  , m_wrapped(&object)
{
}

py::object PyWrapper<Object>::get_parent() const
{
  if (m_wrapped->is_root()) {
    return py::none();
  } else {
    return py::cast(PyWrapper<Object>(m_wrapped->parent()));
  }
}

py::object PyWrapper<Object>::get_children() const
{
  return py::cast(::transform<PyWrapper<Object>>(m_wrapped->children(), [](Object* child) {
    return PyWrapper<Object>(*child);
  }));
}

py::object PyWrapper<Object>::get_tags() const
{
  return py::cast(::transform<PyWrapper<Tag>>(m_wrapped->tags.items(), [](Tag* tag) {
    return PyWrapper<Tag>(*tag);
  }));
}

// -- Style ---

PyWrapper<Style>::PyWrapper(Style& style)
  : PropertyOwnerPyWrapper(style)
  , m_wrapped(&style)
{
}

}  // namespace omm