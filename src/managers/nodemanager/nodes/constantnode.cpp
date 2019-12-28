#include "managers/nodemanager/nodes/constantnode.h"
#include "variant.h"
#include "scene/scene.h"
#include "properties/optionsproperty.h"
#include "managers/propertymanager/userpropertydialog.h"

namespace omm
{

ConstantNode::ConstantNode(Scene* scene)
  : Node(scene)
{
}

void ConstantNode::populate_menu(QMenu& menu)
{
  QAction* edit_port_action = menu.addAction(tr("Edit ports ..."));
  connect(edit_port_action, &QAction::triggered, [this, &menu]() {
    auto dialog = UserPropertyDialog(*this, &menu);
    dialog.exec();
  });
}

std::unique_ptr<Node> ConstantNode::clone() const
{
  return std::make_unique<ConstantNode>(*this);
}

void ConstantNode::on_property_value_changed(Property* property)
{
}

}  // namespace
