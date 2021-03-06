#include "nodesystem/nodesowner.h"
#include "nodesystem/nodemodel.h"
#include "properties/triggerproperty.h"
#include "managers/nodemanager/nodemanager.h"
#include "mainwindow/application.h"

namespace omm
{

NodesOwner::NodesOwner(AbstractNodeCompiler::Language language, Scene& scene)
  : m_node_model(NodeModel::make(language, scene))
{
}

NodesOwner::NodesOwner(const NodesOwner& other)
  : m_node_model(other.node_model() ? std::make_unique<NodeModel>(*other.node_model())
                                    : nullptr)
{
}

NodesOwner::~NodesOwner()
{
}

void NodesOwner::connect_edit_property(TriggerProperty& property, QObject& self)
{
  self.connect(&property, &Property::value_changed, &self, [this]() {
    Manager& manager = Application::instance().get_active_manager(NodeManager::TYPE);
    if (m_node_model) {
      QTimer::singleShot(1, [this, &manager]() {
        static_cast<NodeManager&>(manager).set_model(m_node_model.get());
      });
    }
  });
}

NodeModel* NodesOwner::node_model() const
{
  return m_node_model.get();
}

}  // namespace omm

