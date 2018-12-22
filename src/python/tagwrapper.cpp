#include "python/tagwrapper.h"

namespace omm
{

py::object TagWrapper::owner() const
{
  Object* owner = wrapped<Tag>().owner();
  LOG(INFO) << owner;
  return wrap(owner);
}

void TagWrapper::define_python_interface(py::object& module)
{
  py::class_<TagWrapper, PropertyOwnerWrapper>(module, wrapped_type::TYPE)
        .def("owner", &TagWrapper::owner);
}

}  // namespace omm