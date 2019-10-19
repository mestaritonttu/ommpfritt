#include "preferences/uicolorspage.h"
#include "ui_uicolorspage.h"
#include "preferences/uicolorstreeviewdelegate.h"
#include "preferences/uicolors.h"
#include <QApplication>
#include <QMessageBox>
#include "logging.h"

namespace omm
{

UiColorsPage::UiColorsPage(UiColors& colors)
  : m_ui(new Ui::UiColorsPage)
  , m_colors(colors)
{
  m_ui->setupUi(this);

  std::vector<std::unique_ptr<AbstractPreferencesTreeViewDelegate>> delegates(3);
  for (std::size_t i = 0; i < 3; ++i) {
    delegates.at(i) = std::make_unique<UiColorsTreeViewDelegate>();
  }
  m_ui->treeView->set_model(colors, std::move(delegates));

#ifdef NDEBUG
  // the save as button is only for developing purposes.
  // loading an invalid color schema is likely to result in a crash.
  m_ui->pb_saveas->hide();
#else
  connect(m_ui->pb_saveas, &QPushButton::clicked, this, [this]() {
    m_colors.save_to_file("/tmp/colors.cfg");
  });
#endif

  connect(m_ui->pb_reset, &QPushButton::clicked, this, [this]() {
    if (QMessageBox::question(this, tr("Reset all key bindings"),
        tr("Do you really want to reset all colors for in the current color schema?\nThis cannot be undone."))
        == QMessageBox::Yes)
    {
      m_colors.reset();
    }
  });
  m_colors.store();

  m_ui->comboBox->clear();
  m_ui->comboBox->addItems(m_colors.skin_names());
  m_ui->comboBox->setCurrentIndex(m_colors.skin_index());
  connect(m_ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(load_skin(int)));
}

UiColorsPage::~UiColorsPage()
{
}

void UiColorsPage::about_to_accept()
{
  m_ui->treeView->transfer_editor_data_to_model();
  m_colors.apply();
}

void UiColorsPage::about_to_reject()
{
  m_colors.restore();
}

void UiColorsPage::load_skin(int index)
{
  m_ui->comboBox->setCurrentIndex(index);
  m_colors.set_skin(index);
}

}  // namespace omm