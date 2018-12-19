#pragma once

#include <string>
#include <pybind11/embed.h>

namespace omm
{

class Scene;

class PythonEngine
{
public:
  explicit PythonEngine();
  ~PythonEngine();

  void run(Scene& scene) const;

private:
  // the scoped_interpeter has same lifetime as the application.
  // otherwise, e.g., importing numpy causes crashed.
  // see https://pybind11.readthedocs.io/en/stable/advanced/embedding.html#interpreter-lifetime
  pybind11::scoped_interpreter m_guard;

  PythonEngine(const PythonEngine&) = delete;
  PythonEngine(PythonEngine&&) = delete;

};

}  // namespace omm