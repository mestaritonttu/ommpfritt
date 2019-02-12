#include "renderers/styleiconengine.h"
#include <QPainter>
#include <memory>
#include <glog/logging.h>
#include <iostream>
#include "renderers/viewportrenderer.h"
#include <QDebug>

namespace
{

double adjust_pen_width(double width, const QSize& size)
{
  const double max_width = std::min(size.width(), size.height());

  // f_a(x) := ax/(x+a)
  // f(0) = 0, f(inf) = a,  f'(0) = 1
  // i.e., for small widths (aka x), the function behaves like identity.
  // for large widths, it converges agains max_width (aka a).
  return max_width * width / (max_width + width);
}

void draw_background(QPainter& painter, QRect rect)
{
  static const std::array<QColor, 2> bg_colors = { QColor(128, 128, 128), QColor(180, 180, 180) };

  int size = 7;

  painter.save();
  int mx = rect.right();
  int my = rect.bottom();
  for (int x = rect.left(); x < rect.right(); x += size) {
    for (int y = rect.top(); y < rect.bottom(); y += size) {
      QRect rect(QPoint(x, y), QPoint(std::min(mx, x+size), std::min(my, y+size)));
      painter.fillRect(rect, bg_colors[(x/size+y/size)%bg_colors.size()]);
    }
  }
  painter.restore();
}

void draw_style(QPainter& painter, const QRect& rect, const omm::Style& style)
{
  painter.save();
  painter.setBrush(omm::ViewportRenderer::make_brush(style));
  auto pen = omm::ViewportRenderer::make_pen(style);
  pen.setWidthF(adjust_pen_width(pen.width(), rect.size()));
  painter.setPen(pen);
  const int r = 0.8 * std::min(rect.size().width(), rect.size().height());
  painter.drawEllipse(rect.bottomRight(), r, r);

  painter.restore();
}

}

namespace omm
{

StyleIconEngine::StyleIconEngine(const Style& style) : m_style(style) {}
QIconEngine* StyleIconEngine::clone() const
{
  return std::make_unique<StyleIconEngine>(*this).release();
}

void
StyleIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
  painter->save();
  painter->setClipRect(rect);
  draw_background(*painter, rect);
  draw_style(*painter, rect, m_style);
  painter->restore();
}


}