#include "managers/objectmanager/tagsitemdelegate.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

#include "managers/objectmanager/objecttreeview.h"
#include "tags/tag.h"
#include "scene/scene.h"
#include "managers/objectmanager/objecttreeselectionmodel.h"
#include "mainwindow/application.h"

namespace omm
{

TagsItemDelegate::TagsItemDelegate(ObjectTreeView& view, ObjectTreeSelectionModel& selection_model, const int column)
  : m_view(view)
  , m_selection_model(selection_model)
  , m_column(column)
{
}

void TagsItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem& option,
                              const QModelIndex &index ) const
{
  painter->save();
  QPoint pos = cell_pos(index);

  QPen pen;
  pen.setWidth(4);
  pen.setColor(ui_color(*option.widget, QPalette::Highlight));
  painter->setPen(pen);

  const auto& object = m_view.model()->item_at(index);

  for (size_t i = 0; i < object.tags.size(); ++i)
  {
    const QRect tag_rect = this->tag_rect(pos, i);
    auto& tag = object.tags.item(i);
    painter->setClipRect(tag_rect);
    const QIcon icon = Application::instance().icon_provider.icon(tag);
    icon.paint(painter, tag_rect);

    const bool is_selected = [this, tag_rect, &tag]() {
      const bool tmp_selected = rubberband.intersects(tag_rect);
      const bool selected = m_selection_model.is_selected(tag);
      switch (selection_flag) {
      case QItemSelectionModel::Select:
        return tmp_selected || selected;
      case QItemSelectionModel::Toggle:
        return tmp_selected != selected;
      case QItemSelectionModel::Deselect:
        return selected && !tmp_selected;
      default:
        qFatal("Unexpected selection command");
        Q_UNREACHABLE();
        return false;
      };
    }();

    if (is_selected) {
      painter->drawRect(tag_rect);
    }
  }

  painter->restore();
}

QSize
TagsItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const
{
  const int n_tags = static_cast<int>(m_view.model()->item_at(index).tags.size());
  const int width = n_tags * advance();
  return QSize(width, tag_icon_size().height());
}

bool TagsItemDelegate::editorEvent( QEvent *event, QAbstractItemModel *,
                                    const QStyleOptionViewItem &, const QModelIndex & )
{
  switch (event->type()) {
  case QEvent::MouseButtonPress:
    return on_mouse_button_press(*static_cast<QMouseEvent*>(event));
  case QEvent::MouseButtonRelease:
    return on_mouse_button_release(*static_cast<QMouseEvent*>(event));
  default:
    return false;
  }
}

bool TagsItemDelegate::on_mouse_button_press(QMouseEvent& event)
{
  Tag* tag = tag_at(event.pos());
  if (tag == nullptr) { return false; }
  const bool is_selected = m_selection_model.is_selected(*tag);
  switch (event.button()) {
  case Qt::LeftButton:
    if (!is_selected) {
      if (event.modifiers() & Qt::ControlModifier) {
        m_selection_model.select(*tag, QItemSelectionModel::Select);
      } else if (event.modifiers() & Qt::ShiftModifier) {
        m_selection_model.extend_selection(*tag);
      } else {
        m_selection_model.select(*tag, QItemSelectionModel::ClearAndSelect);
      }
      m_fragile_selection = true;
      return true;
    } else {
      m_fragile_selection = false;
      return false;
    }
  case Qt::RightButton:
    if (!is_selected) {
      m_selection_model.select(*tag, QItemSelectionModel::ClearAndSelect);
      m_fragile_selection = true;
      return true;
    } else {
      m_fragile_selection = false;
      return false;
    }
  default:
    return false;
  }
}

bool TagsItemDelegate::on_mouse_button_release(QMouseEvent& event)
{
  if (event.button() != Qt::LeftButton) { return false; }
  Tag* tag = tag_at(event.pos());
  if (tag == nullptr) { return false; }
  if (m_fragile_selection) {
    m_fragile_selection = false;
    return false;
  }
  if (event.modifiers() & Qt::ControlModifier) {
    m_selection_model.select(*tag, QItemSelectionModel::Toggle);
  } else {
    m_selection_model.select(*tag, QItemSelectionModel::ClearAndSelect);
  }
  m_fragile_selection = false;

  return true;
}

QPoint TagsItemDelegate::cell_pos(const QModelIndex& index) const
{
  return m_view.visualRect(index).topLeft();
}

Tag* TagsItemDelegate::tag_at(const QPoint& pos) const { return tag_at(m_view.indexAt(pos), pos); }

Tag* TagsItemDelegate::tag_at(const QModelIndex& index, const QPoint& pos) const
{
  if (!index.isValid()) { return nullptr; }
  const int x = (pos.x() - cell_pos(index).x()) / advance();
  Object& object = m_view.model()->item_at(index);
  if (x < 0 || x >= static_cast<int>(object.tags.size())) {
    return nullptr;
  } else {
    return &object.tags.item(static_cast<std::size_t>(x));
  }
}

Tag* TagsItemDelegate::tag_before(const QPoint& pos) const
{
  return tag_before(m_view.indexAt(pos), pos);
}

Tag* TagsItemDelegate::tag_before(const QModelIndex& index, QPoint pos) const
{
  if (!index.isValid()) { return nullptr; }

  pos -= QPoint(cell_pos(index).x(), 0);

  Object& object = m_view.model()->item_at(index);
  auto tags = object.tags.ordered_items();
  const int x = int(double(pos.x()) / advance() + 0.5) - 1;
  if (x < 0 || tags.size() == 0) {
    return nullptr;
  } else {
    return tags.at(std::min(tags.size() - 1, static_cast<std::size_t>(x)));
  }
}

QSize TagsItemDelegate::tag_icon_size() const
{
  return QSize(ObjectTreeView::row_height, ObjectTreeView::row_height);
}

int TagsItemDelegate::advance() const
{
  return tag_icon_size().width();
}

std::set<Tag*> TagsItemDelegate::tags(const QModelIndex& index, const QRect& rect) const
{
  const Object& object = m_view.model()->item_at(index);
  const QPoint pos = cell_pos(index.siblingAtColumn(m_column));
  std::set<Tag*> tags;
  for (std::size_t i = 0; i < object.tags.size(); ++i) {
    if (tag_rect(pos, i).intersects(rect)) {
      tags.insert(&object.tags.item(i));
    }
  }
  return tags;
}

QRect TagsItemDelegate::tag_rect(const QPoint& base, std::size_t i) const
{
  return QRect(base + QPoint(i * advance(), 0), tag_icon_size());
}

}  // namespace omm
