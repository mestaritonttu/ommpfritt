#include <QAbstractItemDelegate>
#include <memory>
#include "scene/history/macro.h"
#include "commands/command.h"

namespace omm
{

class QuickAccessDelegate : public QAbstractItemDelegate
{
public:
  explicit QuickAccessDelegate(QAbstractItemView& view);
  void paint( QPainter *painter, const QStyleOptionViewItem &option,
              const QModelIndex &index ) const override;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

  bool on_mouse_button_press(QMouseEvent& event);
  void on_mouse_move(QMouseEvent& event);
  void on_mouse_release(QMouseEvent& event);

  class Area
  {
  public:
    explicit Area(const QRectF& area);
    virtual void draw(QPainter& painter, const QModelIndex& index) = 0;
    const QRectF area;
    bool is_active = false;
    virtual void begin(const QModelIndex& index, QMouseEvent& event) = 0;
    virtual void end() = 0;
    virtual void perform(const QModelIndex& index, QMouseEvent& event) = 0;
  };

protected:
  void add_area(std::unique_ptr<Area> area);

private:
  QAbstractItemView& m_view;

  // a mouse click commits a command.
  // when the mousebutton is hold down and moved, that command is undone and a macro is started.
  // when the mousebutton is released again, the macro is ended.
  std::unique_ptr<Macro> m_macro;
  std::unique_ptr<Command> m_command_on_hold;

  std::list<std::unique_ptr<Area>> m_areas;
  QPointF to_local(const QPoint &view_global, const QModelIndex &index) const;
};

}  // namespace omm