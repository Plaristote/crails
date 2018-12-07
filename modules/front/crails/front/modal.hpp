#ifndef  CRAILS_FRONT_MODAL_HPP
# define CRAILS_FRONT_MODAL_HPP

# include <crails/front/element.hpp>
# include <crails/front/signal_promise.hpp>
# include <memory>

namespace Crails
{
  namespace Front
  {
    class ModalService;

    class Modal : public Crails::Front::Element
    {
      friend class ModalService;
    public:
      Crails::Signal<void> completed;
      Crails::Signal<void> rejected;

      Modal();

      Crails::Front::Promise open();

      void complete() { completed.trigger(); detach(); }
      void reject()   { rejected.trigger();  detach(); }
      
      virtual void close() { complete(); }

      void detach();

    protected:
      Crails::Front::Element content;
      Crails::Front::Element controls, buttons;
      Crails::Front::Element close_button;
    private:
      Crails::Front::SignalPromise promise;
    };

    template<typename RESPONSE_TYPE>
    class ModalDialog : public Modal
    {
    protected:
      RESPONSE_TYPE response;
    public:
      const RESPONSE_TYPE& get_response() const { return response; }
      
      void complete(RESPONSE_TYPE value)
      {
        response = value;
        Modal::complete();
      }

      void reject(RESPONSE_TYPE value)
      {
        response = value;
        Modal::reject();
      }
      
      void close()
      {
        complete(response);
        detach();
      }
    };

    class ModalService
    {
      friend class Modal; 
      typedef std::shared_ptr<Modal> ModalPtr;
      typedef std::vector<Modal*>    ModalList;
      typedef std::vector<ModalPtr>  ModalRefs;

      static ModalList list;
      static ModalRefs references;
      static Crails::Front::Element container;
    public:
      static void initialize();
      
      template<typename MODAL_TYPE>
      static std::shared_ptr<MODAL_TYPE> create()
      {
        auto  modal = std::make_shared<MODAL_TYPE>();
        auto* ptr   = modal.get();

        references.push_back(modal);
        modal->promise.then([ptr]() { remove_modal(ptr); });
        return modal;
      }

    private:
      static void add_modal(Modal* modal);
      static void remove_modal(Modal* modal);
      static void remove_reference(Modal* modal);
      static Modal* get_current_modal();
    };
  }
}

#endif
