#include <crails/front/modal.hpp>

using namespace Crails::Front;
using namespace std;

ModalService::ModalList ModalService::list;
ModalService::ModalRefs ModalService::references;
Element                 ModalService::container;

/*
 * Modal
 */
Modal::Modal() : promise(completed, rejected)
{
  typedef Crails::Front::Element El;

  add_class("modal");
  controls = El("div", {{"class","modal-controls"}});
  buttons  = El("div", {{"class","modal-buttons"}});
  content  = El("div", {{"class","modal-content"}});

  close_button = El("button").text("X");
  close_button.events->on("click", [this]() { close(); });

  controls.inner({
    close_button
  });

  inner({
    controls,
    content,
    buttons
  });
}

Promise Modal::open()
{
  ModalService::add_modal(this);
  return promise;
}

void Modal::detach()
{
  ModalService::remove_modal(this);
  destroy();
}

/*
 * ModalService
 */
void ModalService::initialize()
{
  static bool modal_initialized = false;

  if (!modal_initialized)
  {
    container = Crails::Front::Element("div", {{"class","modal-container"}});
    container.visible(false);
    Crails::Front::body > container;
    modal_initialized = true;
  }
}

void ModalService::add_modal(Modal* modal)
{
  Modal* current_modal = get_current_modal();

  initialize();
  if (current_modal != nullptr)
    current_modal->visible(false);
  list.push_back(modal);
  container > (*modal);
  container.visible(true);
}

void ModalService::remove_modal(Modal* modal)
{
  Modal* current_modal;
  auto   iterator = std::find(list.begin(), list.end(), modal);

  if (iterator != list.end())
  {
    list.erase(iterator);
    current_modal = get_current_modal();
    if (current_modal != nullptr)
      current_modal->visible(true);
    else
      container.visible(false);
  }
  remove_reference(modal);
}

void ModalService::remove_reference(Modal* modal)
{
  for (auto it = references.begin() ; it != references.end() ; ++it)
  {
    if (it->get() == modal)
    {
      references.erase(it);
      break ;
    }
  }
}

Modal* ModalService::get_current_modal()
{
  if (list.size() > 0)
    return *list.rbegin();
  return nullptr;
}
