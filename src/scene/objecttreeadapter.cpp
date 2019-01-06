#include "scene/objecttreeadapter.h"

#include <QItemSelection>
#include <glog/logging.h>

#include "scene/propertyownermimedata.h"
#include "objects/object.h"
#include "common.h"
#include "commands/movecommand.h"
#include "commands/copycommand.h"
#include "properties/stringproperty.h"
#include "scene/scene.h"
#include "abstractraiiguard.h"

namespace
{

class AbstractRAIISceneInvalidatorGuard : public AbstractRAIIGuard
{
protected:
  AbstractRAIISceneInvalidatorGuard(omm::Scene& scene) : m_scene(scene) {}
  ~AbstractRAIISceneInvalidatorGuard() override { m_scene.invalidate(); }

private:
  omm::Scene& m_scene;
};

}  // namespace

namespace omm
{

ObjectTreeAdapter::ObjectTreeAdapter(Scene& scene, Tree<Object>& tree)
  : ItemModelAdapter(scene, tree)
{

}

QModelIndex ObjectTreeAdapter::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  return createIndex(row, column, &item_at(parent).child(row));
}

QModelIndex ObjectTreeAdapter::parent(const QModelIndex& index) const
{
  if (!index.isValid()) {
    return QModelIndex();
  }

  const Object& parent_item = item_at(index);
  if (parent_item.is_root()) {
    return QModelIndex();
  } else {
    return index_of(parent_item.parent());
  }
}

int ObjectTreeAdapter::rowCount(const QModelIndex& parent) const
{
  return item_at(parent).n_children();
}

int ObjectTreeAdapter::columnCount(const QModelIndex& parent) const
{
  return 3;
}

bool ObjectTreeAdapter::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid() || role != Qt::EditRole) {
    return false;
  }

  switch (index.column()) {
  case 0:
    item_at(index).property(Object::NAME_PROPERTY_KEY).set(value.toString().toStdString());
    return true;
  }

  return false;
}

QVariant ObjectTreeAdapter::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  switch (index.column()) {
  case 0:
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
      return QString::fromStdString(item_at(index).name());;
    }
  }
  return QVariant();
}

Object& ObjectTreeAdapter::item_at(const QModelIndex& index) const
{
  if (index.isValid()) {
    assert(index.internalPointer() != nullptr);
    return *static_cast<Object*>(index.internalPointer());
  } else {
    return structure.root();
  }
}

QModelIndex ObjectTreeAdapter::index_of(Object& object) const
{
  if (object.is_root()) {
    return QModelIndex();
  } else {
    return createIndex(scene.object_tree.position(object), 0, &object);
  }
}

QVariant ObjectTreeAdapter::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch (section) {
    case 0: return "object";
    case 1: return "is visible";
    case 2: return "tags";
    }
  }

 return QVariant();
}

Qt::ItemFlags ObjectTreeAdapter::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return Qt::ItemIsDropEnabled;
  }

  switch (index.column()) {
  case 0:
    return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled
            | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  default:
    return Qt::ItemIsEnabled;
  }
}

std::unique_ptr<AbstractRAIIGuard>
ObjectTreeAdapter::acquire_inserter_guard(Object& parent, int row)
{
  class InserterGuard : public AbstractRAIISceneInvalidatorGuard
  {
  public:
    InserterGuard(ObjectTreeAdapter& model, const QModelIndex& parent, int row)
      : AbstractRAIISceneInvalidatorGuard(model.scene)
      , m_model(model)

    {
      m_model.beginInsertRows(parent, row, row);
    }
    ~InserterGuard() { m_model.endInsertRows(); }
  private:
    ObjectTreeAdapter& m_model;
  };
  return std::make_unique<InserterGuard>(*this, index_of(parent), row);
}

std::unique_ptr<AbstractRAIIGuard>
ObjectTreeAdapter::acquire_mover_guard(const ObjectTreeMoveContext& context)
{
  class MoverGuard : public AbstractRAIISceneInvalidatorGuard
  {
  public:
    MoverGuard( ObjectTreeAdapter& model, const QModelIndex& old_parent, const int old_pos,
                const QModelIndex& new_parent, const int new_pos )
      : AbstractRAIISceneInvalidatorGuard(model.scene)
      , m_model(model)
    {
      m_model.beginMoveRows(old_parent, old_pos, old_pos, new_parent, new_pos);
    }

    ~MoverGuard() { m_model.endMoveRows(); }
  private:
    ObjectTreeAdapter& m_model;
  };

  assert(!context.subject.get().is_root());
  Object& old_parent = context.subject.get().parent();
  Object& new_parent = context.parent.get();
  const auto old_pos = scene.object_tree.position(context.subject);
  const auto new_pos = scene.object_tree.insert_position(context.predecessor);

  if (old_pos == new_pos && &old_parent == &new_parent) {
    return nullptr;
  } else {
    return std::make_unique<MoverGuard>( *this, index_of(old_parent), old_pos,
                                         index_of(new_parent), new_pos );
  }
}

std::unique_ptr<AbstractRAIIGuard>
ObjectTreeAdapter::acquire_remover_guard(const Object& object)
{
  class RemoverGuard : public AbstractRAIISceneInvalidatorGuard
  {
  public:
    RemoverGuard(ObjectTreeAdapter& model, const QModelIndex& parent, int row)
      : AbstractRAIISceneInvalidatorGuard(model.scene)
      , m_model(model)
    {
      m_model.beginRemoveRows(parent, row, row);
    }
    ~RemoverGuard() { m_model.endRemoveRows(); }

  private:
    ObjectTreeAdapter& m_model;
  };
  return std::make_unique<RemoverGuard>( *this, index_of(object.parent()),
                                         scene.object_tree.position(object) );
}

std::unique_ptr<AbstractRAIIGuard>
ObjectTreeAdapter::acquire_reseter_guard()
{
  class ReseterGuard : public AbstractRAIISceneInvalidatorGuard
  {
  public:
    ReseterGuard(ObjectTreeAdapter& model)
      : AbstractRAIISceneInvalidatorGuard(model.scene)
      , m_model(model)
    {
      m_model.beginResetModel();
    }
    ~ReseterGuard() { m_model.endResetModel(); }

  private:
    ObjectTreeAdapter& m_model;
  };
  return std::make_unique<ReseterGuard>(*this);
}

}  // namespace omm
