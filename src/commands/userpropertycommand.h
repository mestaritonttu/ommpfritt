#pragma once

#include "commands/command.h"
#include <vector>
#include <memory>
#include "properties/property.h"

namespace omm
{

class DisconnectPortsCommand;
class ConnectPortsCommand;
class AbstractPropertyOwner;
class NodeModel;

class UserPropertyCommand : public Command
{
public:
  using pmap = std::map<QString, std::unique_ptr<Property>>;
  using cmap = std::map<QString, Property::Configuration>;

  UserPropertyCommand(const std::vector<QString>& deletions,
                      std::vector<std::pair<QString, std::unique_ptr<Property>>> additions,
                      const std::map<Property*, Property::Configuration>& changes,
                      AbstractPropertyOwner& owner);
  ~UserPropertyCommand();
  void undo() override;
  void redo() override;

private:
  using Properties = std::vector<std::pair<QString, std::unique_ptr<Property>>>;
  Properties m_deletions;
  Properties m_additions;
  using Configurations = std::map<Property*, Property::Configuration>;
  Configurations m_changes;

  AbstractPropertyOwner& m_owner;
  std::set<std::unique_ptr<DisconnectPortsCommand>> m_broken_connections;

  void extract(Properties& ps);
  void insert(Properties& ps);
  void change(Configurations& configurations);

  void swap();
  NodeModel* m_node_model = nullptr;
};

}  // namespace omm
