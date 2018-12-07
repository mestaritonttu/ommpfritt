#include "scene/list.h"
#include <algorithm>
#include "scene/contextes.h"
#include "renderers/style.h"

namespace
{
// TODO same in Tree
using Guard = std::unique_ptr<AbstractRAIIGuard>;
}  // namespace

namespace omm
{

template<typename T> std::set<T*> List<T>::items() const
{
  return ::transform<T*, std::set>(m_items, [](const auto& item) { return item.get(); });
}

template<typename T> T& List<T>::item(size_t i) const
{
  return *m_items[i].get();
}

template<typename T> std::set<T*> List<T>::selected_items() const
{
  // TODO same in Tree
  const auto is_selected = [](const auto* t) { return t->is_selected(); };
  return ::filter_if(items(), is_selected);
}

template<typename T> void List<T>::insert(std::unique_ptr<T> item)
{
  const auto guards = observed_type::template transform<Guard>(
    [this](auto* observer){ return observer->acquire_inserter_guard(m_items.size()); }
  );
  m_items.push_back(std::move(item));
  // selection_changed();  // TODO
}

template<typename T> void List<T>::insert(ListOwningContext<T>& context)
{
  const size_t position = nullptr ? 0 : this->position(*context.predecessor) + 1;
  const auto guards = observed_type::template transform<Guard>(
    [this, position](auto* observer) {
      return observer->acquire_inserter_guard(position);
    }
  );
  m_items.insert(m_items.begin() + position, context.subject.release());
  // selection_changed();  // TODO
}

template<typename T> void List<T>::remove(ListOwningContext<T>& context)
{
  const auto guards = observed_type::template transform<Guard>(
    [this, &context](auto* observer) {
      return observer->acquire_remover_guard(position(context.subject));
    }
  );
  context.subject.capture(::extract(m_items, context.subject.reference()));
  // selection_changed();  // TODO
}

template<typename T> std::unique_ptr<T> List<T>::remove(T& item)
{
  ;
  const auto guards = observed_type::template transform<Guard>(
    [this, &item](auto* observer){ return observer->acquire_remover_guard(position(item)); }
  );
  return ::extract(m_items, item);
}

template<typename T> size_t List<T>::position(const T& item) const
{
  const auto pos = std::find_if(m_items.begin(), m_items.end(), [&item](const auto& uptr) {
    return uptr.get() == &item;
  });

  assert(pos != m_items.end());
  return std::distance(pos, m_items.begin());
}

template<typename T> const T* List<T>::predecessor(const T& item) const
{
  const auto pos = this->position(item);
  if (pos == 0) {
    return nullptr;
  } else {
    return m_items.at(pos - 1).get();
  }
}

template<typename T> void List<T>::move(ListMoveContext<T>& context)
{
  // TODO
}

template class List<Style>;

}  // namespace omm