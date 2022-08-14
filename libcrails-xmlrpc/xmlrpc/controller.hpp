#ifndef  XMLRPC_CONTROLLER_HPP
# define XMLRPC_CONTROLLER_HPP

# ifdef CRAILS_B2_INCLUDE
# include "../../libcrails-controllers/controller.hpp"
# include "params.hpp"
# else
# include "../controller.hpp"
# include "../params.hpp"
# endif
# include <boost/fusion/functional/invocation/invoke.hpp>
# include <boost/fusion/container.hpp>
# include "variable.hpp"

namespace XmlRpc
{
  class Controller : public Crails::Controller
  {
    struct IMethod
    {
      IMethod(const std::string& name) : name(name) {}

      const std::string name;
      virtual void call(Crails::Params&) = 0;
    };

    template<typename KLASS, typename ...Args>
    struct Method : public IMethod
    {
      typedef void (KLASS::*MethodType)(Args...);

      struct Callback
      {
        KLASS&     object;
        MethodType callback;

        Callback(KLASS& object, MethodType callback) : object(object), callback(callback)
        {}

        void operator()(Args... args)
        {
          (object.*callback)(args...);
        }
      };

      Callback callback;

      Method(const std::string& name, KLASS& object, MethodType callback) : IMethod(name), callback(object, callback)
      {
      }

      void call(Crails::Params& params)
      {
        typedef boost::fusion::vector<Args...> fusion_vector;
        const Data xmlrpc_params = params["document"]["methodCall"]["params"];
        fusion_vector          method_params;
        std::vector<Variable>  method_variables;

        xmlrpc_params.each([&method_variables](const Data param) -> bool
        {
          const Data value = param.at(0);

          method_variables.push_back(Variable::from_data(value));
          return true;
        });

        set_fusion_iterator(
          boost::fusion::begin(method_params), boost::fusion::end(method_params),
          method_variables.begin(),
          boost::fusion::result_of::equal_to<
            typename boost::fusion::result_of::end<fusion_vector>::type,
            typename boost::fusion::result_of::begin<fusion_vector>::type
          >()
        );

        boost::fusion::invoke(callback, method_params);
      }
    private:
      // this will be called when we reach end of the fusion container(FIBeginT==FIEndT)
      template< class FIBeginT, class FIEndT, class StdIteratorT >
      static void set_fusion_iterator( FIBeginT b, FIEndT e, StdIteratorT i, boost::mpl::true_ )
      {
      }
      // this will be called when FIBeginT != FIEndT
      template< class FIBeginT, class FIEndT, class StdIteratorT >
      static void set_fusion_iterator( FIBeginT b, FIEndT e, StdIteratorT i, boost::mpl::false_ )
      {
        using namespace boost;
        *b = *i;
        set_fusion_iterator( fusion::next(b), e, ++i,
          fusion::result_of::equal_to<
            typename fusion::result_of::next<FIBeginT>::type, FIEndT >()
        );
      }
    };

  public:
    Controller(Crails::Context& context) : Crails::Controller(context)
    {
    }

    void endpoint();

    virtual bool must_protect_from_forgery() const { return false; }

  protected:
    void respond_with(const XmlRpc::Variable&);
    void respond_with_fault(int code, const std::string& message);
    void raise_unknown_method();

    template<typename KLASS, typename ...Args>
    void register_method(const std::string& name,  void (KLASS::*method)(Args...))
    {
      methods.push_back(std::unique_ptr<IMethod>(new Method<KLASS, Args...>(name, reinterpret_cast<KLASS&>(*this), method)));
    }

    std::vector< std::unique_ptr<IMethod> > methods;
  };
}

#endif
