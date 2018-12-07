#pragma once

#include <vector>
#include <memory>
#include <set>
#include "scene/contextes_fwd.h"
#include "scene/structure.h"
#include "scene/abstractstructureobserver.h"

namespace omm
{

template<typename T>
class List : public Structure<T>, public Observed<AbstractStructureObserver<List<T>>>
{
public:
  using observer_type = AbstractStructureObserver<List<T>>;
  using observed_type = Observed<observer_type>;

  void insert(std::unique_ptr<T> t);
  void insert(ListOwningContext<T>& context);
  std::unique_ptr<T> remove(T& t); // TODO remove?
  void remove(ListOwningContext<T>& t);
  void move(ListMoveContext<T>& context);
  std::set<T*> items() const;
  T& item(size_t i) const;
  std::set<T*> selected_items() const;
  size_t position(const T& item) const;
  const T* predecessor(const T& sibling) const override;

private:
  std::vector<std::unique_ptr<Style>> m_items;
};

}  // namespace omm