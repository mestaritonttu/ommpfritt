#include "animation/animator.h"

#include "logging.h"
#include "serializers/abstractserializer.h"
#include "animation/track.h"

#include "aspects/propertyowner.h"

namespace omm
{

Animator::Animator()
{
  m_timer.setInterval(1000.0/30.0);
  connect(&m_timer, SIGNAL(timeout()), this, SLOT(advance()));
  connect(this, SIGNAL(current_changed(int)), this, SLOT(apply()));
}

Animator::~Animator()
{

}

void Animator::serialize(AbstractSerializer &serializer, const Serializable::Pointer &pointer) const
{
  Serializable::serialize(serializer, pointer);

  serializer.set_value(m_start_frame, make_pointer(pointer, START_FRAME_POINTER));
  serializer.set_value(m_end_frame, make_pointer(pointer, END_FRAME_POINTER));
  serializer.set_value(m_current_frame, make_pointer(pointer, CURRENT_FRAME_POINTER));

  const auto tracks_pointer = make_pointer(pointer, TRACKS_POINTER);
  serializer.start_array(m_tracks.size(), tracks_pointer);
  int i = 0;
  for (const auto& track : m_tracks) {
    track->serialize(serializer, make_pointer(tracks_pointer, i));
    i += 1;
  }
  serializer.end_array();
}

void Animator::deserialize(AbstractDeserializer &deserializer, const Pointer &pointer)
{
  Serializable::deserialize(deserializer, pointer);

  set_start(deserializer.get_int(make_pointer(pointer, START_FRAME_POINTER)));
  set_end(deserializer.get_int(make_pointer(pointer, END_FRAME_POINTER)));
  set_current(deserializer.get_int(make_pointer(pointer, CURRENT_FRAME_POINTER)));

  const auto tracks_pointer = make_pointer(pointer, TRACKS_POINTER);
  const std::size_t n = deserializer.array_size(tracks_pointer);
  for (std::size_t i = 0; i < n; ++i) {
    auto track = std::make_unique<Track>();
    track->deserialize(deserializer, make_pointer(tracks_pointer, i));
    m_tracks.insert(std::move(track));
  }
}

Track* Animator::track(AbstractPropertyOwner &owner, const std::string &property_key) const
{
  const auto it = std::find_if(m_tracks.begin(), m_tracks.end(),
                               [&owner, &property_key](const auto& fcurve)
  {
    return fcurve->owner() == &owner && fcurve->property_key() == property_key;
  });

  if (it != m_tracks.end()) {
    return it->get();
  } else {
    return nullptr;
  }
}

Track *Animator::create_track(AbstractPropertyOwner &owner, const std::string &property_key)
{
  // no duplicates!
  assert(this->track(owner, property_key) == nullptr);

  auto track = std::make_unique<Track>();
  LINFO << track->owner();
  track->set_owner(owner, property_key);
  Track& track_ref = *track;
  connect(&track_ref, SIGNAL(track_changed()), this, SIGNAL(tracks_changed()));
  m_tracks.insert(std::move(track));
  Q_EMIT tracks_changed();
  return &track_ref;
}

std::unique_ptr<Track> Animator::extract_track(AbstractPropertyOwner &owner,
                                                       const std::string &property_key)
{
  const auto it = std::find_if(m_tracks.begin(), m_tracks.end(), [&](const auto& t) {
    return t->owner() == &owner && t->property_key() == property_key;
  });
  std::unique_ptr<Track> track = std::move(m_tracks.extract(it).value());
  Q_EMIT tracks_changed();
  return track;
}

void Animator::set_start(int start)
{
  if (m_start_frame != start) {
    m_start_frame = start;
    Q_EMIT start_changed(start);
  }
}

void Animator::set_end(int end)
{
  if (m_end_frame != end) {
    m_end_frame = end;
    Q_EMIT end_changed(end);
  }
}

void Animator::set_current(int current)
{
  if (m_current_frame != current) {
    m_current_frame = current;
    Q_EMIT current_changed(current);
  }
}

void Animator::toggle_play_pause(bool play)
{
  if (m_is_playing != play) {
    m_is_playing = play;
    if (play) {
      m_timer.start();
    } else {
      m_timer.stop();
    }
    Q_EMIT play_pause_toggled(play);
  }
}

void Animator::advance()
{
  int next = m_current_frame + 1;
  if (next > m_end_frame) {
    if (m_play_mode == PlayMode::Repeat) {
      next = m_start_frame;
    } else if (m_play_mode == PlayMode::Stop) {
      next = m_end_frame;
      m_timer.stop();
    }
  }

  if (next != m_current_frame) {
    set_current(next);
  }
}

void Animator::apply()
{
  for (const auto& track : m_tracks) {
    track->apply(m_current_frame);
  }
}

}  // namespace omm
