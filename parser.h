#ifndef parser_h
#define parser_h

#include <vector>
#include <utility>

#include "range/v3/algorithm/search.hpp"
#include "gsl/gsl"
#include "bpstd/string_view.h"

using namespace ranges;
using namespace std;

namespace utils
{
   using string_t      = bpstd::string_view;
   using string_view_t = bpstd::string_view;
   //using string_t      = string;

   bool starts_with(bpstd::string_view src, bpstd::string_view _search)
   {
      auto ret = v3::search(src, _search);
      return ((ret == src.begin()) ? true : false);
   }

   bool is_switch(bpstd::string_view arg)
   {
      return starts_with(arg, "-") or starts_with(arg, "--");
   }

   struct switch_iters
   {
      using iter   = vector<bpstd::string_view>::iterator;
      using args_t = vector<bpstd::string_view>;

      /// construct a begin and end from a string (e.g from a -m,
      /// points to beginning of -m till the end of the last -m arg)
      switch_iters(args_t& args)
      {
         for (size_t i = 0; i < args.size(); ++i)
         {
            if (is_switch(args[i]))
               iters.push_back(args.begin() + i);
         }
      }

      iter at(size_t idx) { return iters[idx]; }
      iter& operator[](size_t idx) { return iters[idx]; }
      const iter& operator[](size_t idx) const { return iters[idx]; }

      vector<iter> iters;
   };

   struct switch_iter
   {
      using iterator_t = vector<string_t>::iterator;
      using args_t     = vector<string_t>;

      /// construct a begin and end from a string (e.g from a -m,
      /// points to beginning of -m till the end of the last -m arg)
      switch_iter(args_t& args, string_t swtch)
      {
         construct(args, swtch);
      }

      void construct(args_t&, string_t);

      iterator_t begin() { return iter_beg; }
      iterator_t end()   { return iter_end; }

      string_t operator[](int idx) { return *(iter_beg + idx); }
      auto size() const { return iter_end - iter_beg; }

      iterator_t iter_beg, iter_end;
   };

   void switch_iter::construct(args_t& args, string_t swtch)
   {
      for (size_t i = 0; i < args.size(); ++i)
      {
         if (is_switch(swtch) && (args[i] == swtch))
         {
            iter_beg = args.begin() + (i + 1);
            while(i++ != args.size())
            {
               if (is_switch(args[i]))
               {
                  iter_end = args.begin() + (i);
                  break;
               }
            }
            break;
         }
      }
   }

   struct cmd
   {
      cmd() = default;
      inline cmd(int argc, char* argv[]);

      void add_arg(string_view_t sv) { m_args.push_back(sv.data()); }
      void assign(int argc, char* argv[]);

      auto begin() { return &m_args[0]; }
      auto end()   { return &m_args[m_args.size()];  }

      auto& all_args() { return m_args; }

      vector<string_t> m_args;
   };

   cmd::cmd(int argc, char* argv[])
   {
      for (int i = 0; i < argc; ++i)
         m_args.push_back(argv[i]);
   }

   void cmd::assign(int argc, char* argv[])
   {
      m_args.clear();
      for (int i = 0; i < argc; ++i)
         m_args.push_back(argv[i]);
   }

   void run2(int argc, char* argv[])
   {
      cmd cm(argc, argv);
      switch_iter s(cm.all_args(), "-h");
      switch_iter s2(cm.all_args(), "--help");
      switch_iter s3(cm.all_args(), "--hlp");

      for (auto c : s)
         cout << c << '\n';

      cout << '\n';

      for (auto c : s2)
         cout << c << '\n';

      for (int i = 0; i < s3.size(); ++i)
         cout << s3[i] <<'\n';
   }

   void main(int argc, char* argv[])
   {
      run2(argc, argv);
   }
}

#endif//parser_h
