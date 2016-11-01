#include <crails/front/local_storage.hpp>
#include <cheerp/client.h>
#include <iostream>

using namespace std;
using namespace Crails::Front;

LocalStorage::LocalStorage(client::Storage* storage) :
  storage(storage)
{
  initialize();
}

LocalStorage::LocalStorage()
{
  storage = client::window.get_localStorage();
  initialize();
}

LocalStorage::~LocalStorage()
{
  client::window.removeEventListener("storage", event_listener);
}

void LocalStorage::initialize()
{
  event_listener = cheerp::Callback([this](client::Event* _event)
  {
    auto* event = static_cast<client::StorageEvent*>(_event);

    if (event->get_storageArea() == storage)
      on_change.trigger(Change(event));
  });
  client::window.addEventListener("storage", event_listener);
}

LocalStorage::Change::Change(client::StorageEvent* event)
{
  key = (string)(*event->get_key());
  if (event->get_oldValue())
    old_value = (string)(*static_cast<client::String*>(event->get_oldValue()));
  if (event->get_newValue())
    new_value = (string)(*static_cast<client::String*>(event->get_newValue()));
}

template<>
void LocalStorage::set_item<const string&>(const string& key, const string& value)
{
  storage->setItem(key.c_str(), value.c_str());
}

template<>
void LocalStorage::set_item<string>(const string& key, string value)
{
  set_item<const string&>(key, value);
}

template<>
string LocalStorage::get_item<string>(const string& key) const
{
  client::Object* object = storage->getItem(key.c_str());

  if (object)
    return (string)(*static_cast<client::String*>(object));
  return "";
}

void LocalStorage::remove_item(const string& key)
{
  storage->removeItem(key.c_str());
}

void LocalStorage::clear()
{
  storage->clear();
}
