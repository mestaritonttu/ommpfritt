#include "preferences/keybindingspage.h"
#include <QMessageBox>
#include <QTableView>
#include <QDialogButtonBox>
#include <memory>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QHeaderView>
#include "keybindings/keybindings.h"
#include "preferences/keybindingstreeviewdelegate.h"
#include <QSettings>
#include "ui_keybindingspage.h"
#include <QStyle>

namespace omm
{

KeyBindingsPage::KeyBindingsPage(KeyBindings& key_bindings, QWidget* parent)
  : PreferencePage(parent)
  , m_proxy_model(KeyBindingsProxyModel(key_bindings))
  , m_ui(std::make_unique<Ui::KeyBindingsPage>())
  , m_key_bindings(key_bindings)
{
  m_ui->setupUi(this);
  std::vector<std::unique_ptr<AbstractPreferencesTreeViewDelegate>> delegates(1);
  delegates.at(0) = std::make_unique<KeyBindingsTreeViewDelegate>();
  m_ui->treeView->set_model(m_proxy_model, std::move(delegates));
  m_ui->treeView->header()->hide();
  connect(m_ui->pb_reset, SIGNAL(clicked()), this, SLOT(reset()));
  connect(m_ui->pb_reset_filter, SIGNAL(clicked()), this, SLOT(reset_filter()));
  connect(m_ui->le_name_filter, SIGNAL(textChanged(const QString&)),
          this, SLOT(set_name_filter(const QString&)));
  connect(m_ui->le_sequence_filter, SIGNAL(keySequenceChanged(const QKeySequence&)),
          this, SLOT(set_sequence_filter(const QKeySequence&)));
  m_ui->pb_reset_filter->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));

  QSettings settings;
  settings.beginGroup(KEYBINDINGS_DIALOG_SETTINGS_GROUP);
  restoreGeometry(settings.value(GEOMETRY_SETTINGS_KEY).toByteArray());
  settings.endGroup();
  key_bindings.store();
}

KeyBindingsPage::~KeyBindingsPage()
{
  QSettings settings;
  settings.beginGroup(KEYBINDINGS_DIALOG_SETTINGS_GROUP);
  settings.setValue(GEOMETRY_SETTINGS_KEY, saveGeometry());
  settings.endGroup();
}

void KeyBindingsPage::about_to_accept()
{
  m_ui->treeView->transfer_editor_data_to_model();
}

void KeyBindingsPage::about_to_reject()
{
  m_key_bindings.restore();
}

void KeyBindingsPage::update_expand()
{
  const int count = m_proxy_model.rowCount(QModelIndex());
  for (int row = 0; row < count; ++row) {
    const QModelIndex index = m_proxy_model.index(row, 0, QModelIndex());
    if (m_proxy_model.rowCount(index) > 0) {
      m_ui->treeView->expand(index);
    }
  }
}

void KeyBindingsPage::reset()
{
  if (QMessageBox::question(this, tr("Reset all key bindings"),
      tr("Do you really want to reset all key bindings to the default value?\nThis cannot be undone."))
      == QMessageBox::Yes)
  {
    m_key_bindings.reset();
  }
}

void KeyBindingsPage::reset_filter()
{
  m_ui->le_name_filter->clear();
  m_ui->le_sequence_filter->clear();
}

void KeyBindingsPage::set_name_filter(const QString& filter)
{
  m_proxy_model.set_action_name_filter(filter.toStdString());
  update_expand();
}

void KeyBindingsPage::set_sequence_filter(const QKeySequence& sequence)
{
  m_proxy_model.set_action_sequence_filter(sequence.toString(QKeySequence::NativeText).toStdString());
  update_expand();
}

}  // namespace omm
