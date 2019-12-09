#pragma once

#include "managers/nodemanager/panzoomcontroller.h"
#include "cachedgetter.h"
#include <QWidget>
#include "managers/range.h"

class QPainter;

namespace omm
{

class NodeModel;
class InputPort;
class Port;
class Node;

class NodeView : public QWidget
{
  Q_OBJECT

public:
  explicit NodeView(QWidget* parent = nullptr);
  ~NodeView();
  void set_model(NodeModel* model);
  NodeModel* model() const { return m_model; }
  const QFont font;
  void abort();
  void remove_selection();

protected:
  void paintEvent(QPaintEvent*) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* e) override;

private:
  NodeModel* m_model = nullptr;
  PanZoomController m_pzc;
  QPoint offset() const;
  void draw_node(QPainter& painter, const Node& node) const;
  void draw_connection(QPainter& painter, const InputPort& input_port) const;
  void draw_port(QPainter& painter, const Port& port) const;
  void draw_connection(QPainter& painter, const QPointF& in, const QPointF& out) const;
  QPointF port_pos(const Port& port) const;
  QRectF node_geometry(const Node& node) const;
  Port* port(std::set<Port*> candidates, const QPointF& pos) const;
  bool select_port_or_node(const QPoint pos, bool extend_selection);

  class CachedNodeWidthGetter : public ArgsCachedGetter<double, NodeView, const Node*>
  {
  public:
    explicit CachedNodeWidthGetter(NodeView& node_view);
  protected:
    double compute(const Node* node) const override;
  private:
    const QFontMetricsF m_font_metrics;
  } node_width_cache;

  Port* m_tmp_connection_origin = nullptr;
  Port* m_tmp_connection_target = nullptr;
  Port* m_former_connection_target = nullptr;
  InputPort* m_about_to_disconnect = nullptr;
  bool m_aborted = false;
  std::set<Node*> m_selection;

};

}  // namespace omm
