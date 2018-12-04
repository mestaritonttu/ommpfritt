#include "managers/stylemanager/stylelistadapter.h"
#include "renderers/style.h"

namespace omm
{

StyleListAdapter::StyleListAdapter(Scene& scene)
  : m_scene(scene)
{
  m_scene.Observed<AbstractStyleListObserver>::register_observer(*this);
}

StyleListAdapter::~StyleListAdapter()
{
  m_scene.Observed<AbstractStyleListObserver>::unregister_observer(*this);
}

int StyleListAdapter::rowCount(const QModelIndex& parent) const
{
  assert(!parent.isValid());
  return m_scene.styles().size();
}

QVariant StyleListAdapter::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  assert(!index.parent().isValid());

  switch (role) {
  case Qt::DisplayRole:
  case Qt::EditRole:
    return QString::fromStdString(m_scene.style(index.row()).name());
  }
  return QVariant();
}

Qt::ItemFlags StyleListAdapter::flags(const QModelIndex &index) const
{
  assert(!index.parent().isValid());
  return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled
            | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}


Scene& StyleListAdapter::scene() const
{
  return m_scene;
}

std::unique_ptr<AbstractStyleListObserver::AbstractInserterGuard>
StyleListAdapter::acquire_inserter_guard(int row)
{
  class InserterGuard : public AbstractInserterGuard
  {
  public:
    InserterGuard(StyleListAdapter& model, int row) : m_model(model)
    {
      m_model.beginInsertRows(QModelIndex(), row, row);
    }
    ~InserterGuard() { m_model.endInsertRows(); }
  private:
    StyleListAdapter& m_model;
  };
  return std::make_unique<InserterGuard>(*this, row);
}

  // friend class AbstractMoverGuard;
  // std::unique_ptr<AbstractMoverGuard> acquire_mover_guard() override;

std::unique_ptr<AbstractStyleListObserver::AbstractRemoverGuard>
StyleListAdapter::acquire_remover_guard(int row)
{
  class RemoverGuard : public AbstractRemoverGuard
  {
  public:
    RemoverGuard(StyleListAdapter& model, int row) : m_model(model)
    {
      m_model.beginRemoveRows(QModelIndex(), row, row);
    }
    ~RemoverGuard() { m_model.endRemoveRows(); }
  private:
    StyleListAdapter& m_model;
  };
  return std::make_unique<RemoverGuard>(*this, row);
}

std::unique_ptr<AbstractStyleListObserver::AbstractReseterGuard>
StyleListAdapter::acquire_reseter_guard()
{
  class ReseterGuard : public AbstractReseterGuard
  {
  public:
    ReseterGuard(StyleListAdapter& model) : m_model(model)
    {
      m_model.beginResetModel();
    }
    ~ReseterGuard() { m_model.endResetModel(); }
  private:
    StyleListAdapter& m_model;
  };
  return std::make_unique<ReseterGuard>(*this);
}

bool StyleListAdapter::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid() || role != Qt::EditRole) {
    return false;
  }

  assert(index.column() == 0);
  assert(!index.parent().isValid());

  auto& style = m_scene.style(index.row());
  auto& name_property = style.property<StringProperty>(Object::NAME_PROPERTY_KEY);
  name_property.set_value(value.toString().toStdString());
  return true;
}

}  // namespace omm