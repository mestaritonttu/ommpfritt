#pragma once

#include <QAbstractItemModel>
#include "scene/structure.h"
#include "objects/object.h"
#include "scene/list.h"

namespace omm
{

class Scene;

template<typename StructureT, typename ItemT, typename ItemModel>
class ItemModelAdapter : public ItemModel
{
  static_assert( std::is_base_of<QAbstractItemModel, ItemModel>::value,
                 "ItemModel must be derived from QAbstractItemModel" );
public:
  using structure_type = StructureT;
  explicit ItemModelAdapter(Scene& scene, StructureT& structure);
  virtual ~ItemModelAdapter() = default;
  Qt::DropActions supportedDragActions() const override;
  Qt::DropActions supportedDropActions() const override;
  bool canDropMimeData( const QMimeData *data, Qt::DropAction action,
                        int row, int column, const QModelIndex &parent ) const override;
  bool dropMimeData( const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent ) override;
  QStringList mimeTypes() const override;
  QMimeData* mimeData(const QModelIndexList &indexes) const override;
  virtual ItemT& item_at(const QModelIndex& index) const = 0;
  virtual QModelIndex index_of(ItemT& item) const = 0;
  Scene& scene;
  StructureT& structure;

private:
};

}  // namespace omm
