#include "managers/propertymanager/userpropertymanager/propertyconfigwidget.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include "properties/property.h"
#include <QPushButton>
#include "managers/propertymanager/userpropertymanager/propertyitem.h"

namespace
{

template<typename Ts> auto to_vector(const Ts& ts)
{
  const auto f = [](auto&& s) { return std::forward<decltype(s)>(s); };
  return ::transform<std::string, std::vector>(ts, f);
}

}  // namespace

namespace omm
{

AbstractPropertyConfigWidget::AbstractPropertyConfigWidget(QWidget* parent, Property& property)
  : QWidget(parent)
  , m_property_types(to_vector(Property::keys()))
  , m_property(property)
{
  setLayout(std::make_unique<QVBoxLayout>(this).release());

  m_type_combobox = std::make_unique<QComboBox>(this).release();
  m_type_combobox->addItems(::transform<QString, QList>(m_property_types, QString::fromStdString));
  layout()->addWidget(m_type_combobox);
  set_property_type(property.type());

  m_name_edit = std::make_unique<QLineEdit>(this).release();
  layout()->addWidget(m_name_edit);
  m_name_edit->setPlaceholderText(PropertyItem::UNNAMED_PROPERTY_PLACEHOLDER);
  m_name_edit->setText(QString::fromStdString(property.label()));

  connect(m_name_edit, &QLineEdit::textChanged, [this](const QString& text) {
    m_property.set_label(text.toStdString());
    Q_EMIT property_label_changed();
  });

  layout()->addWidget(std::make_unique<QPushButton>("XXX", this).release());


  const auto index_changed = static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
  connect(m_type_combobox, index_changed, [this](int index) {
    const auto type = m_property_types[index];
    Q_EMIT property_type_changed(type);
  });
}

void AbstractPropertyConfigWidget::set_property_type(const std::string& type)
{
  const auto pos = std::find(m_property_types.begin(), m_property_types.end(), type);
  assert(pos != m_property_types.end());
  LOG(INFO) << "set type " << type;
  m_type_combobox->setCurrentIndex(std::distance(m_property_types.begin(), pos));
}

}  // namespace omm