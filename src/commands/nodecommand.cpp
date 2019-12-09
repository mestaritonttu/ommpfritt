#include "managers/nodemanager/port.h"
#include "common.h"
#include "managers/nodemanager/nodemodel.h"
#include "managers/nodemanager/node.h"
#include "commands/nodecommand.h"

namespace
{

std::map<omm::Node*, QPointF> collect_old_positions(const std::set<omm::Node*>& nodes)
{
  std::map<omm::Node*, QPointF> map;
  for (omm::Node* node : nodes) {
    map.insert({ node, node->pos() });
  }
  return map;
}

}  // namespace

namespace omm
{

ConnectionCommand::ConnectionCommand(const QString& label, Port& a, Port& b)
  : Command(label)
  , m_out(static_cast<OutputPort&>(b.is_input ? a : b))
  , m_in(static_cast<InputPort&>(a.is_input ? a : b))
{
  // require exactly one input and one output.
  assert(a.is_input != b.is_input);
}

void ConnectionCommand::connect()
{
  m_in.connect(&m_out);
}

void ConnectionCommand::disconnect()
{
  m_in.connect(nullptr);
}

ConnectPortsCommand::ConnectPortsCommand(Port& a, Port& b)
  : ConnectionCommand(QObject::tr("Connect Ports"), a, b)
{
}

DisconnectPortsCommand::DisconnectPortsCommand(InputPort& port)
  : ConnectionCommand(QObject::tr("Disconnect Ports"), port, *port.connected_output())
{
}


NodeCommand::NodeCommand(const QString& label, NodeModel& model,
                         std::vector<Node*> refs, std::vector<std::unique_ptr<Node>> owns)
  : Command(label)
  , m_refs(refs), m_owns(std::move(owns))
  , m_model(model)
{
  if (m_refs.empty()) {
    for (const auto& node : m_owns) {
      m_refs.push_back(node.get());
    }
  }

#ifndef NDEBUG
  if (!m_owns.empty()) {
    assert(m_refs.size() == m_owns.size());
    for (std::size_t i = 0; i < m_refs.size(); ++i) {
      assert(m_refs.at(i) == m_owns.at(i).get());
    }
  }
#endif
}

void NodeCommand::remove()
{
  m_owns.reserve(m_refs.size());
  for (Node* node : m_refs) {
    for (Port* port : node->ports()) {
      std::set<InputPort*> ips;
      OutputPort* op = nullptr;
      if (port->is_input) {
        InputPort* ip = static_cast<InputPort*>(port);
        op = ip->connected_output();
        ips.insert(ip);
      } else {
        op = static_cast<OutputPort*>(port);
        ips = op->connected_inputs();
      }
      for (InputPort* ip : ips) {
        m_destroyed_connections.emplace_back(*ip);
        m_destroyed_connections.back().redo();
      }
    }
    m_owns.push_back(m_model.extract_node(*node));
  }
}

void NodeCommand::add()
{
  for (auto&& node : m_owns) {
    m_model.add_node(std::move(node));
  }
  for (auto it = m_destroyed_connections.rbegin(); it != m_destroyed_connections.rend(); ++it) {
    it->undo();
  }
  m_destroyed_connections.clear();
  m_owns.clear();
}

RemoveNodesCommand::RemoveNodesCommand(NodeModel& model, std::vector<Node*> nodes)
  : NodeCommand(QObject::tr("Remove Nodes"), model, nodes, {})
{
}

AddNodesCommand::AddNodesCommand(NodeModel& model, std::vector<std::unique_ptr<Node>> nodes)
  : NodeCommand(QObject::tr("Add Nodes"), model, {}, std::move(nodes))
{
}

MoveNodesCommand::MoveNodesCommand(std::set<Node*> nodes, const QPointF& direction)
  : Command(QObject::tr("Move Nodes"))
  , m_old_positions(collect_old_positions(nodes))
  , m_direction(direction)
{
}

void MoveNodesCommand::undo()
{
  for (auto&& [ node, old_pos ] : m_old_positions) {
    node->set_pos(old_pos);
  }
}

void MoveNodesCommand::redo()
{
  for (auto&& [ node, old_pos ] : m_old_positions) {
    node->set_pos(old_pos + m_direction);
  }
}

bool MoveNodesCommand::mergeWith(const QUndoCommand* command)
{
  const MoveNodesCommand& mn_command = static_cast<const MoveNodesCommand&>(*command);
  if (::same_keys(mn_command.m_old_positions, m_old_positions)) {
    m_direction += mn_command.m_direction;
    return true;
  } else {
    return false;
  }
}

}  // namespace omm
