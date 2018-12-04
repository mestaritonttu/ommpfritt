#pragma once

#include <memory>
#include <vector>
#include "commands/command.h"

namespace omm
{

class Tag;
class Object;

class AttachTagCommand : public Command
{
public:
  AttachTagCommand(Scene& scene, std::unique_ptr<Tag> tag);

private:
  void undo() override;
  void redo() override;

  class TagContext;
  std::vector<TagContext> m_contextes;
  Scene& m_scene;
};


class AttachTagCommand::TagContext
{
public:
  TagContext(Object& object, std::unique_ptr<Tag> tag);

  Object* tag_owner;
  std::unique_ptr<Tag> owned;
  Tag* reference;
};




}  // namespace omm