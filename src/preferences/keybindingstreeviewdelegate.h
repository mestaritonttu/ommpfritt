#pragma once

#include <QKeySequenceEdit>
#include <memory>
#include "preferences/preferencestreeview.h"
#include "preferences/keysequenceedit.h"
#include "preferences/preferencestreeviewdelegate.h"

namespace omm
{

class KeyBindingsTreeViewDelegate : public PreferencesTreeViewDelegate<KeySequenceEdit>
{
public:
  using PreferencesTreeViewDelegate::PreferencesTreeViewDelegate;
protected:
  void set_editor_data(omm::KeySequenceEdit &editor, const QModelIndex &index) const override;
  bool set_model_data(omm::KeySequenceEdit &editor, QAbstractItemModel &model,
                      const QModelIndex &index) const override;

};

}  // namespace omm
