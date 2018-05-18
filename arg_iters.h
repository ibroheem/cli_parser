#ifndef args_iters_h
#define args_iters_h

#undef __cplusplus
#define __cplusplus 201703

#include <utility>
#include <type_traits>

//#include "range/v3/algorithm/search.hpp"
#include "nonstd/string_view.h"

//using namespace ranges;

namespace cli
{
   using string_view   = bpstd::string_view;
   using string_t      = bpstd::string_view;
   using string_view_t = bpstd::string_view;

   using argv_t  = char**;
   using argc_t  = int;
   using index_t = int;
   using value_t = char*;

   enum class errors { index_err = -1 };

   bool starts_with(string_view src, string_view _search)
   {
      //auto ret = v3::search(src, _search);
      auto ret = std::search(src.begin(), src.end(), _search.begin(), _search.end());
      return ((ret == src.begin()) ? true : false);
   }

   bool is_switch(string_view arg)
   {
      return starts_with(arg, "-") or starts_with(arg, "--");
   }

   struct args_iter
   {
      constexpr args_iter(argc_t argc, argv_t argv)
         : iter_beg(argv), iter_end(argv + argc)
      {
      }

      /// construct a begin and end from a string (e.g from a -m,
      /// points to beginning of -m till the end of the last -m arg)
      args_iter(argc_t argc, argv_t argv, string_t swtch)
         : iter_beg(argv + construct(argc, argv, swtch)), iter_end(argv + argc)
      {

      }

      static int construct(argc_t argc, argv_t argv, string_t swtch)
      {
         int i = 0;
         for (; i < argc; ++i)
            if (is_switch(swtch) && (argv[i] == swtch))
               break;

         return i;
      }

      template <typename T>
      constexpr auto index(T);//  -> conditional_t<is_same_v<T, argc_t>, argv_t, argc_t>;

      constexpr int     operator[] (string_t );
      constexpr value_t operator[] (argc_t );

      argv_t begin() { return iter_beg; }
      argv_t end()   { return iter_end; }

      auto size() const { return iter_end - iter_beg; }

      /// gets the value of an args (i.e the next arg after it)
      template <typename T>
      const auto next(T arg)
      {
         int i = index(arg);
         if (i != -1)
            return *(iter_beg + i + 1);

         return *iter_end;
      }

      template <typename T = string_view>
      T value(string_view);

      argv_t iter_beg, iter_end;
   };

   /// Returns index if a switch is passed, returns arg value if index is passed
   template <typename T>
   constexpr auto args_iter::index(T arg)
   {
      if constexpr (std::is_same_v<T, string_t>)
      {
         int i = 0;
         for (auto && c : *this)
         {
            if (arg == c)
               return i;
            ++i;
         }
         return -1;
      }
      else
      {
         if ((iter_beg + arg) < iter_end)
            return *(iter_beg + arg);
         else
         {
            return *iter_end;
         }
      }
   }

   constexpr int args_iter::operator[] (string_t swtch)
   {
      return index(swtch);
   }

   constexpr value_t args_iter::operator[] (argc_t argc)
   {
      return index(argc);
   }

   template <typename T>
   T args_iter::value(string_view arg)
   {
      char* ret = next(arg);
      /** Inner index() gets the index : int, the outer use the value to locate the value */
      if constexpr (std::is_integral_v<T>)
      {
         /// TODO: Handle other integral types
         if (ret)
            return atoi(ret);
      }
      else
      {
         if (ret)
            return ret;
      }
      //throw std::runtime_error("No Value For Specified Arguement");
   }
}

#ifdef ARG_ITER_TESTS
namespace cli::tests
{
   void test1(int argc, char** argv)
   {
      args_iter args(argc, argv);
      args_iter names(argc, argv, "-n");

      char* ns = args.value<char*>("-n");

      fmt::fprintf(std::cout, "-t: %s\n-c: %s\n-n: %s\n-n[1]: %s\n",
                   args.value<int>("-t"),
                   args.value<int>("-c"),
                   args.value<char*>("-n"),
                   ++ns);

      fmt::print("\nNames:\n");
      for (auto e : names)
         fmt::print("{}\n", e);
   }

   void main(int argc, char** argv)
   {
      test1(argc, argv);
   }
}
#endif

#endif//args_iters_h
