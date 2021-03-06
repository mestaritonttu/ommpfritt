#pragma once

#include "preferences/keybindingsproxymodel.h"
#include <memory>
#include "preferences/preferencepage.h"

namespace Ui { class KeyBindingsPage; }

namespace omm
{

class KeyBindings;

class KeyBindingsPage  : public PreferencePage
{
  Q_OBJECT
public:
  explicit KeyBindingsPage(KeyBindings& key_bindings, QWidget* parent = nullptr);
  ~KeyBindingsPage();

  static constexpr auto KEYBINDINGS_DIALOG_SETTINGS_GROUP = "keybindingsdialog";
  static constexpr auto GEOMETRY_SETTINGS_KEY = "geometry";

  void about_to_accept() override;
  void about_to_reject() override;

private:
  KeyBindingsProxyModel m_proxy_model;
  std::unique_ptr<Ui::KeyBindingsPage> m_ui;
  KeyBindings& m_key_bindings;
  std::map<QString, std::map<QString, QKeySequence>> m_revert_cache;
  void update_expand();

private Q_SLOTS:
  void reset();
  void reset_filter();
  void set_name_filter(const QString& filter);
  void set_sequence_filter(const QKeySequence& sequence);
};

}  // namespace omm
