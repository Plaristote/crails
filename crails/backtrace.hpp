//
//  Copyright (c) 2010 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_BACKTRACE_HPP
#define BOOST_BACKTRACE_HPP

#include <boost/config.hpp>
#include <stdexcept>
#include <typeinfo>
#include <vector>
#include <iosfwd>
#include <string>

namespace boost_ext {

    namespace stack_trace {
        int trace(void **addresses,int size);
        void write_symbols(void *const *addresses,int size,std::ostream &);
        std::string get_symbol(void *address);
        std::string get_symbols(void * const *address,int size);
    } // stack_trace

    class backtrace {
    public:
        
        static size_t const default_stack_size = 32;

        backtrace(size_t frames_no = default_stack_size) 
        {
            if(frames_no == 0)
                return;
            frames_.resize(frames_no,0);
            int size = stack_trace::trace(&frames_.front(),frames_no);
            frames_.resize(size);
        }

        virtual ~backtrace() throw()
        {
        }

        size_t stack_size() const
        {
            return frames_.size();
        }

        void *return_address(unsigned frame_no) const
        {
            if(frame_no < stack_size())
                return frames_[frame_no];
            return 0;
        }

        void trace_line(unsigned frame_no,std::ostream &out) const
        {
            if(frame_no < frames_.size())
                stack_trace::write_symbols(&frames_[frame_no],1,out);
        }

        std::string trace_line(unsigned frame_no) const
        {
            if(frame_no < frames_.size())
                return stack_trace::get_symbol(frames_[frame_no]);
            return std::string();
        }

        std::string trace() const
        {
            if(frames_.empty())
                return std::string();
            return stack_trace::get_symbols(&frames_.front(),frames_.size());
        }

        void trace(std::ostream &out) const
        {
            if(frames_.empty())
                return;
            stack_trace::write_symbols(&frames_.front(),frames_.size(),out);
        }
    
    private:
        std::vector<void *> frames_;
    };

    class exception : public std::exception, public backtrace {
    public:
    };
    
    class bad_cast : public std::bad_cast, public backtrace {
    public:
    };

    class runtime_error: public std::runtime_error, public backtrace {
    public:
        explicit runtime_error(std::string const &s) : std::runtime_error(s) 
        {
        }
    };

    class range_error: public std::range_error, public backtrace {
    public:
        explicit range_error(std::string const &s) : std::range_error(s) 
        {
        }
    };

    class overflow_error: public std::overflow_error, public backtrace {
    public:
        explicit overflow_error(std::string const &s) : std::overflow_error(s) 
        {
        }
    };

    class underflow_error: public std::underflow_error, public backtrace {
    public:
        explicit underflow_error(std::string const &s) : std::underflow_error(s) 
        {
        }
    };

    class logic_error: public std::logic_error, public backtrace {
    public:
        explicit logic_error(std::string const &s) : std::logic_error(s) 
        {
        }
    };

    class domain_error: public std::domain_error, public backtrace {
    public:
        explicit domain_error(std::string const &s) : std::domain_error(s) 
        {
        }
    };

    class length_error: public std::length_error, public backtrace {
    public:
        explicit length_error(std::string const &s) : std::length_error(s) 
        {
        }
    };

    class invalid_argument : public std::invalid_argument, public backtrace {
    public:
        explicit invalid_argument(std::string const &s) : std::invalid_argument(s)
        {
        }
    };
    
    class out_of_range : public std::out_of_range, public backtrace {
    public:
        explicit out_of_range(std::string const &s) : std::out_of_range(s)
        {
        }
    };

    namespace details {
        class trace_manip {
        public:
            trace_manip(backtrace const *tr) :
                tr_(tr)
            {
            }
            std::ostream &write(std::ostream &out) const
            {
                if(tr_)
                    tr_->trace(out);
                return out;
            }
        private:
            backtrace const *tr_;
        };

        inline std::ostream &operator<<(std::ostream &out,details::trace_manip const &t)
        {
            return t.write(out);
        }
    }

    template<typename E>
    details::trace_manip trace(E const &e)
    {
        backtrace const *tr = dynamic_cast<backtrace const *>(&e);
        return details::trace_manip(tr);
    }


} // boost

#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

