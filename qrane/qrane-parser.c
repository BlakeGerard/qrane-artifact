// A Bison parser, made by GNU Bison 3.3.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2019 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.





#include "qrane-parser.h"


// Unqualified %code blocks.
#line 31 "qrane-parser.y" // lalr1.cc:435

    #define _USE_MATH_DEFINES

    #include <cmath>
    #include <cstddef>
    #include <iostream>
    #include <memory>

    // Declare Flex/Bison variables and functions
    extern yy::qrane_parser::symbol_type yylex();
    extern char *yytext;
    extern yy::location loc;

    bool qreg_seen = false;
    unsigned int qreg_size = 0;
    qop_id qop_count = 0;

#line 63 "qrane-parser.c" // lalr1.cc:435


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
#line 158 "qrane-parser.c" // lalr1.cc:510

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  qrane_parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  qrane_parser::qrane_parser (std::shared_ptr<qrane_mainprogram> mainprogram_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      mainprogram (mainprogram_yyarg)
  {}

  qrane_parser::~qrane_parser ()
  {}

  qrane_parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  qrane_parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  qrane_parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  qrane_parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  qrane_parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  qrane_parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  qrane_parser::symbol_number_type
  qrane_parser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  qrane_parser::stack_symbol_type::stack_symbol_type ()
  {}

  qrane_parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 37: // T_REAL
        value.YY_MOVE_OR_COPY< double > (YY_MOVE (that.value));
        break;

      case 49: // anylist
      case 50: // idlist
      case 51: // mixedlist
        value.YY_MOVE_OR_COPY< qrane_argumentlist > (YY_MOVE (that.value));
        break;

      case 53: // explist
        value.YY_MOVE_OR_COPY< qrane_parameterlist > (YY_MOVE (that.value));
        break;

      case 41: // program
      case 48: // goplist
        value.YY_MOVE_OR_COPY< qrane_statementlist > (YY_MOVE (that.value));
        break;

      case 52: // argument
        value.YY_MOVE_OR_COPY< std::shared_ptr<qrane_argument> > (YY_MOVE (that.value));
        break;

      case 43: // decl
      case 44: // gatedecl
        value.YY_MOVE_OR_COPY< std::shared_ptr<qrane_decl> > (YY_MOVE (that.value));
        break;

      case 40: // mainprogram
        value.YY_MOVE_OR_COPY< std::shared_ptr<qrane_mainprogram> > (YY_MOVE (that.value));
        break;

      case 54: // exp
        value.YY_MOVE_OR_COPY< std::shared_ptr<qrane_parameter> > (YY_MOVE (that.value));
        break;

      case 46: // qop
      case 47: // uop
        value.YY_MOVE_OR_COPY< std::shared_ptr<qrane_qop> > (YY_MOVE (that.value));
        break;

      case 45: // reg
        value.YY_MOVE_OR_COPY< std::shared_ptr<qrane_reg> > (YY_MOVE (that.value));
        break;

      case 42: // statement
        value.YY_MOVE_OR_COPY< std::shared_ptr<qrane_statement> > (YY_MOVE (that.value));
        break;

      case 4: // T_QREG
      case 5: // T_CREG
      case 6: // T_GATE_CUSTOM
      case 7: // T_OPAQUE
      case 8: // T_BARRIER
      case 9: // T_MEASURE
      case 10: // T_RESET
      case 24: // T_ADD
      case 25: // T_SUB
      case 26: // T_MUL
      case 27: // T_DIV
      case 28: // T_CARROT
      case 29: // T_PI
      case 30: // T_SIN
      case 31: // T_COS
      case 32: // T_TAN
      case 33: // T_EXP
      case 34: // T_LN
      case 35: // T_SQRT
      case 36: // T_ID
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case 55: // unaryop
        value.YY_MOVE_OR_COPY< unaryop_type > (YY_MOVE (that.value));
        break;

      case 38: // T_UINTEGER
        value.YY_MOVE_OR_COPY< unsigned int > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  qrane_parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 37: // T_REAL
        value.move< double > (YY_MOVE (that.value));
        break;

      case 49: // anylist
      case 50: // idlist
      case 51: // mixedlist
        value.move< qrane_argumentlist > (YY_MOVE (that.value));
        break;

      case 53: // explist
        value.move< qrane_parameterlist > (YY_MOVE (that.value));
        break;

      case 41: // program
      case 48: // goplist
        value.move< qrane_statementlist > (YY_MOVE (that.value));
        break;

      case 52: // argument
        value.move< std::shared_ptr<qrane_argument> > (YY_MOVE (that.value));
        break;

      case 43: // decl
      case 44: // gatedecl
        value.move< std::shared_ptr<qrane_decl> > (YY_MOVE (that.value));
        break;

      case 40: // mainprogram
        value.move< std::shared_ptr<qrane_mainprogram> > (YY_MOVE (that.value));
        break;

      case 54: // exp
        value.move< std::shared_ptr<qrane_parameter> > (YY_MOVE (that.value));
        break;

      case 46: // qop
      case 47: // uop
        value.move< std::shared_ptr<qrane_qop> > (YY_MOVE (that.value));
        break;

      case 45: // reg
        value.move< std::shared_ptr<qrane_reg> > (YY_MOVE (that.value));
        break;

      case 42: // statement
        value.move< std::shared_ptr<qrane_statement> > (YY_MOVE (that.value));
        break;

      case 4: // T_QREG
      case 5: // T_CREG
      case 6: // T_GATE_CUSTOM
      case 7: // T_OPAQUE
      case 8: // T_BARRIER
      case 9: // T_MEASURE
      case 10: // T_RESET
      case 24: // T_ADD
      case 25: // T_SUB
      case 26: // T_MUL
      case 27: // T_DIV
      case 28: // T_CARROT
      case 29: // T_PI
      case 30: // T_SIN
      case 31: // T_COS
      case 32: // T_TAN
      case 33: // T_EXP
      case 34: // T_LN
      case 35: // T_SQRT
      case 36: // T_ID
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case 55: // unaryop
        value.move< unaryop_type > (YY_MOVE (that.value));
        break;

      case 38: // T_UINTEGER
        value.move< unsigned int > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  qrane_parser::stack_symbol_type&
  qrane_parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 37: // T_REAL
        value.move< double > (that.value);
        break;

      case 49: // anylist
      case 50: // idlist
      case 51: // mixedlist
        value.move< qrane_argumentlist > (that.value);
        break;

      case 53: // explist
        value.move< qrane_parameterlist > (that.value);
        break;

      case 41: // program
      case 48: // goplist
        value.move< qrane_statementlist > (that.value);
        break;

      case 52: // argument
        value.move< std::shared_ptr<qrane_argument> > (that.value);
        break;

      case 43: // decl
      case 44: // gatedecl
        value.move< std::shared_ptr<qrane_decl> > (that.value);
        break;

      case 40: // mainprogram
        value.move< std::shared_ptr<qrane_mainprogram> > (that.value);
        break;

      case 54: // exp
        value.move< std::shared_ptr<qrane_parameter> > (that.value);
        break;

      case 46: // qop
      case 47: // uop
        value.move< std::shared_ptr<qrane_qop> > (that.value);
        break;

      case 45: // reg
        value.move< std::shared_ptr<qrane_reg> > (that.value);
        break;

      case 42: // statement
        value.move< std::shared_ptr<qrane_statement> > (that.value);
        break;

      case 4: // T_QREG
      case 5: // T_CREG
      case 6: // T_GATE_CUSTOM
      case 7: // T_OPAQUE
      case 8: // T_BARRIER
      case 9: // T_MEASURE
      case 10: // T_RESET
      case 24: // T_ADD
      case 25: // T_SUB
      case 26: // T_MUL
      case 27: // T_DIV
      case 28: // T_CARROT
      case 29: // T_PI
      case 30: // T_SIN
      case 31: // T_COS
      case 32: // T_TAN
      case 33: // T_EXP
      case 34: // T_LN
      case 35: // T_SQRT
      case 36: // T_ID
        value.move< std::string > (that.value);
        break;

      case 55: // unaryop
        value.move< unaryop_type > (that.value);
        break;

      case 38: // T_UINTEGER
        value.move< unsigned int > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  qrane_parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  qrane_parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  void
  qrane_parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  qrane_parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  qrane_parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  qrane_parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  qrane_parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  qrane_parser::debug_level_type
  qrane_parser::debug_level () const
  {
    return yydebug_;
  }

  void
  qrane_parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  qrane_parser::state_type
  qrane_parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  qrane_parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  qrane_parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  qrane_parser::operator() ()
  {
    return parse ();
  }

  int
  qrane_parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex ());
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case 37: // T_REAL
        yylhs.value.emplace< double > ();
        break;

      case 49: // anylist
      case 50: // idlist
      case 51: // mixedlist
        yylhs.value.emplace< qrane_argumentlist > ();
        break;

      case 53: // explist
        yylhs.value.emplace< qrane_parameterlist > ();
        break;

      case 41: // program
      case 48: // goplist
        yylhs.value.emplace< qrane_statementlist > ();
        break;

      case 52: // argument
        yylhs.value.emplace< std::shared_ptr<qrane_argument> > ();
        break;

      case 43: // decl
      case 44: // gatedecl
        yylhs.value.emplace< std::shared_ptr<qrane_decl> > ();
        break;

      case 40: // mainprogram
        yylhs.value.emplace< std::shared_ptr<qrane_mainprogram> > ();
        break;

      case 54: // exp
        yylhs.value.emplace< std::shared_ptr<qrane_parameter> > ();
        break;

      case 46: // qop
      case 47: // uop
        yylhs.value.emplace< std::shared_ptr<qrane_qop> > ();
        break;

      case 45: // reg
        yylhs.value.emplace< std::shared_ptr<qrane_reg> > ();
        break;

      case 42: // statement
        yylhs.value.emplace< std::shared_ptr<qrane_statement> > ();
        break;

      case 4: // T_QREG
      case 5: // T_CREG
      case 6: // T_GATE_CUSTOM
      case 7: // T_OPAQUE
      case 8: // T_BARRIER
      case 9: // T_MEASURE
      case 10: // T_RESET
      case 24: // T_ADD
      case 25: // T_SUB
      case 26: // T_MUL
      case 27: // T_DIV
      case 28: // T_CARROT
      case 29: // T_PI
      case 30: // T_SIN
      case 31: // T_COS
      case 32: // T_TAN
      case 33: // T_EXP
      case 34: // T_LN
      case 35: // T_SQRT
      case 36: // T_ID
        yylhs.value.emplace< std::string > ();
        break;

      case 55: // unaryop
        yylhs.value.emplace< unaryop_type > ();
        break;

      case 38: // T_UINTEGER
        yylhs.value.emplace< unsigned int > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2:
#line 116 "qrane-parser.y" // lalr1.cc:919
    {
                mainprogram->initialize(yystack_[0].value.as < qrane_statementlist > (), qreg_size);
            }
#line 890 "qrane-parser.c" // lalr1.cc:919
    break;

  case 3:
#line 121 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_statementlist > () = qrane_statementlist();
            yylhs.value.as < qrane_statementlist > ().append(yystack_[0].value.as < std::shared_ptr<qrane_statement> > ());
        }
#line 899 "qrane-parser.c" // lalr1.cc:919
    break;

  case 4:
#line 126 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_statementlist > () = yystack_[1].value.as < qrane_statementlist > ();
            yylhs.value.as < qrane_statementlist > ().append(yystack_[0].value.as < std::shared_ptr<qrane_statement> > ());
        }
#line 908 "qrane-parser.c" // lalr1.cc:919
    break;

  case 5:
#line 132 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < std::shared_ptr<qrane_statement> > () = std::static_pointer_cast<qrane_statement>(yystack_[0].value.as < std::shared_ptr<qrane_decl> > ());
          }
#line 916 "qrane-parser.c" // lalr1.cc:919
    break;

  case 6:
#line 136 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < std::shared_ptr<qrane_statement> > () = std::static_pointer_cast<qrane_statement>(yystack_[0].value.as < std::shared_ptr<qrane_reg> > ());
          }
#line 924 "qrane-parser.c" // lalr1.cc:919
    break;

  case 7:
#line 140 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < std::shared_ptr<qrane_statement> > () = std::static_pointer_cast<qrane_statement>(yystack_[0].value.as < std::shared_ptr<qrane_qop> > ());
          }
#line 932 "qrane-parser.c" // lalr1.cc:919
    break;

  case 8:
#line 144 "qrane-parser.y" // lalr1.cc:919
    {
              //$$ = $7;
              //$$->set_if_condition($3, $5);
          }
#line 941 "qrane-parser.c" // lalr1.cc:919
    break;

  case 9:
#line 150 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_decl> > () = yystack_[2].value.as < std::shared_ptr<qrane_decl> > ();
        yylhs.value.as < std::shared_ptr<qrane_decl> > ()->set_statementlist(yystack_[1].value.as < qrane_statementlist > ());
     }
#line 950 "qrane-parser.c" // lalr1.cc:919
    break;

  case 10:
#line 155 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_decl> > () = yystack_[1].value.as < std::shared_ptr<qrane_decl> > ();
     }
#line 958 "qrane-parser.c" // lalr1.cc:919
    break;

  case 11:
#line 160 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < std::shared_ptr<qrane_decl> > () = std::make_shared<qrane_decl>(
                    statement_type::DECL, decl_type::GATEDECL,
                    yystack_[2].value.as < std::string > (), yystack_[1].value.as < qrane_argumentlist > ());
         }
#line 968 "qrane-parser.c" // lalr1.cc:919
    break;

  case 12:
#line 166 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < std::shared_ptr<qrane_decl> > () = std::make_shared<qrane_decl>(
                    statement_type::DECL, decl_type::GATEDECL,
                    yystack_[4].value.as < std::string > (), yystack_[1].value.as < qrane_argumentlist > ());
         }
#line 978 "qrane-parser.c" // lalr1.cc:919
    break;

  case 13:
#line 172 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < std::shared_ptr<qrane_decl> > () = std::make_shared<qrane_decl>(
                    statement_type::DECL, decl_type::GATEDECL,
                    yystack_[5].value.as < std::string > (), yystack_[3].value.as < qrane_parameterlist > (), yystack_[1].value.as < qrane_argumentlist > ());
         }
#line 988 "qrane-parser.c" // lalr1.cc:919
    break;

  case 14:
#line 179 "qrane-parser.y" // lalr1.cc:919
    {
        qreg_seen = true;
		qreg_size = yystack_[2].value.as < unsigned int > ();
         yylhs.value.as < std::shared_ptr<qrane_reg> > () = std::make_shared<qrane_reg>(
                statement_type::REG, reg_type::QREG,
                yystack_[5].value.as < std::string > (), yystack_[4].value.as < std::string > (), yystack_[2].value.as < unsigned int > ());
     }
#line 1000 "qrane-parser.c" // lalr1.cc:919
    break;

  case 15:
#line 187 "qrane-parser.y" // lalr1.cc:919
    {
         yylhs.value.as < std::shared_ptr<qrane_reg> > () = std::make_shared<qrane_reg>(
                statement_type::REG, reg_type::CREG,
                yystack_[5].value.as < std::string > (), yystack_[4].value.as < std::string > (), yystack_[2].value.as < unsigned int > ());
     }
#line 1010 "qrane-parser.c" // lalr1.cc:919
    break;

  case 16:
#line 194 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = yystack_[0].value.as < std::shared_ptr<qrane_qop> > ();
    }
#line 1018 "qrane-parser.c" // lalr1.cc:919
    break;

  case 17:
#line 198 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = std::make_shared<qrane_qop>(
                statement_type::QOP, qop_type::MEASURE, 
                qop_count, yystack_[4].value.as < std::string > (), qrane_argumentlist({yystack_[3].value.as < std::shared_ptr<qrane_argument> > (), yystack_[1].value.as < std::shared_ptr<qrane_argument> > ()}));
        if (qreg_seen) { qop_count += 1; }
    }
#line 1029 "qrane-parser.c" // lalr1.cc:919
    break;

  case 18:
#line 205 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = std::make_shared<qrane_qop>(
                statement_type::QOP, qop_type::RESET, 
                qop_count, yystack_[2].value.as < std::string > (), qrane_argumentlist({yystack_[1].value.as < std::shared_ptr<qrane_argument> > ()}));
        if (qreg_seen) { qop_count += 1; }
    }
#line 1040 "qrane-parser.c" // lalr1.cc:919
    break;

  case 19:
#line 212 "qrane-parser.y" // lalr1.cc:919
    {
        if (qreg_seen) { mainprogram->increment_num_qops(); }
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = std::make_shared<qrane_qop>(
                statement_type::QOP, qop_type::BARRIER, 
                qop_count, yystack_[2].value.as < std::string > (), yystack_[1].value.as < qrane_argumentlist > ());
        if (qreg_seen) { qop_count += 1; }
    }
#line 1052 "qrane-parser.c" // lalr1.cc:919
    break;

  case 20:
#line 220 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = std::make_shared<qrane_qop>(
                statement_type::QOP, qop_type::OPAQUE, 
                qop_count, yystack_[2].value.as < std::string > (), yystack_[1].value.as < qrane_argumentlist > ());
        if (qreg_seen) { qop_count += 1; }
    }
#line 1063 "qrane-parser.c" // lalr1.cc:919
    break;

  case 21:
#line 227 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = std::make_shared<qrane_qop>(
                statement_type::QOP, qop_type::OPAQUE, 
                qop_count, yystack_[4].value.as < std::string > (), yystack_[1].value.as < qrane_argumentlist > ());
        if (qreg_seen) { qop_count += 1; }
    }
#line 1074 "qrane-parser.c" // lalr1.cc:919
    break;

  case 22:
#line 234 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = std::make_shared<qrane_qop>(
                statement_type::QOP, qop_type::OPAQUE, 
                qop_count, yystack_[5].value.as < std::string > (), yystack_[3].value.as < qrane_parameterlist > (), yystack_[1].value.as < qrane_argumentlist > ());
        if (qreg_seen) { qop_count += 1; }
    }
#line 1085 "qrane-parser.c" // lalr1.cc:919
    break;

  case 23:
#line 242 "qrane-parser.y" // lalr1.cc:919
    {
		
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = std::make_shared<qrane_qop>(
                statement_type::QOP, qop_type::GATE, 
                qop_count, yystack_[2].value.as < std::string > (), yystack_[1].value.as < qrane_argumentlist > ());
        if (qreg_seen) { qop_count += 1; }
    }
#line 1097 "qrane-parser.c" // lalr1.cc:919
    break;

  case 24:
#line 250 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = std::make_shared<qrane_qop>(
                statement_type::QOP, qop_type::GATE, 
                qop_count, yystack_[4].value.as < std::string > (), yystack_[1].value.as < qrane_argumentlist > ());
        if (qreg_seen) { qop_count += 1; }
    }
#line 1108 "qrane-parser.c" // lalr1.cc:919
    break;

  case 25:
#line 257 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_qop> > () = std::make_shared<qrane_qop>(
                statement_type::QOP, qop_type::GATE, 
                qop_count, yystack_[5].value.as < std::string > (), yystack_[3].value.as < qrane_parameterlist > (), yystack_[1].value.as < qrane_argumentlist > ());
        if (qreg_seen) { qop_count += 1; }
    }
#line 1119 "qrane-parser.c" // lalr1.cc:919
    break;

  case 26:
#line 265 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_statementlist > () = qrane_statementlist();
            yylhs.value.as < qrane_statementlist > ().append(yystack_[0].value.as < std::shared_ptr<qrane_qop> > ()); 
        }
#line 1128 "qrane-parser.c" // lalr1.cc:919
    break;

  case 27:
#line 270 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_statementlist > () = yystack_[1].value.as < qrane_statementlist > ();
            yylhs.value.as < qrane_statementlist > ().append(yystack_[0].value.as < std::shared_ptr<qrane_qop> > ());
        }
#line 1137 "qrane-parser.c" // lalr1.cc:919
    break;

  case 28:
#line 275 "qrane-parser.y" // lalr1.cc:919
    { yylhs.value.as < qrane_argumentlist > () = yystack_[0].value.as < qrane_argumentlist > (); }
#line 1143 "qrane-parser.c" // lalr1.cc:919
    break;

  case 29:
#line 276 "qrane-parser.y" // lalr1.cc:919
    { yylhs.value.as < qrane_argumentlist > () = yystack_[0].value.as < qrane_argumentlist > (); }
#line 1149 "qrane-parser.c" // lalr1.cc:919
    break;

  case 30:
#line 279 "qrane-parser.y" // lalr1.cc:919
    {
           yylhs.value.as < qrane_argumentlist > () = qrane_argumentlist();
           yylhs.value.as < qrane_argumentlist > ().append(std::make_shared<qrane_argument>(yystack_[0].value.as < std::string > ()));
       }
#line 1158 "qrane-parser.c" // lalr1.cc:919
    break;

  case 31:
#line 284 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_argumentlist > () = yystack_[2].value.as < qrane_argumentlist > ();
            yylhs.value.as < qrane_argumentlist > ().append(std::make_shared<qrane_argument>(yystack_[0].value.as < std::string > ()));
       }
#line 1167 "qrane-parser.c" // lalr1.cc:919
    break;

  case 32:
#line 290 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_argumentlist > () = qrane_argumentlist();
            yylhs.value.as < qrane_argumentlist > ().append(std::make_shared<qrane_argument>(yystack_[3].value.as < std::string > (), yystack_[1].value.as < unsigned int > ()));
          }
#line 1176 "qrane-parser.c" // lalr1.cc:919
    break;

  case 33:
#line 295 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_argumentlist > () = yystack_[2].value.as < qrane_argumentlist > ();
            yylhs.value.as < qrane_argumentlist > ().append(std::make_shared<qrane_argument>(yystack_[0].value.as < std::string > ()));
          }
#line 1185 "qrane-parser.c" // lalr1.cc:919
    break;

  case 34:
#line 300 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_argumentlist > () = yystack_[5].value.as < qrane_argumentlist > ();
            yylhs.value.as < qrane_argumentlist > ().append(std::make_shared<qrane_argument>(yystack_[3].value.as < std::string > (), yystack_[1].value.as < unsigned int > ()));
          }
#line 1194 "qrane-parser.c" // lalr1.cc:919
    break;

  case 35:
#line 305 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_argumentlist > () = yystack_[5].value.as < qrane_argumentlist > ();
            yylhs.value.as < qrane_argumentlist > ().append(std::make_shared<qrane_argument>(yystack_[3].value.as < std::string > (), yystack_[1].value.as < unsigned int > ()));
          }
#line 1203 "qrane-parser.c" // lalr1.cc:919
    break;

  case 36:
#line 311 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < std::shared_ptr<qrane_argument> > () = std::make_shared<qrane_argument>(yystack_[0].value.as < std::string > ());
         }
#line 1211 "qrane-parser.c" // lalr1.cc:919
    break;

  case 37:
#line 315 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < std::shared_ptr<qrane_argument> > () = std::make_shared<qrane_argument>(yystack_[3].value.as < std::string > (), yystack_[1].value.as < unsigned int > ());
         }
#line 1219 "qrane-parser.c" // lalr1.cc:919
    break;

  case 38:
#line 320 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_parameterlist > () = qrane_parameterlist();
            yylhs.value.as < qrane_parameterlist > ().append(yystack_[0].value.as < std::shared_ptr<qrane_parameter> > ());
        }
#line 1228 "qrane-parser.c" // lalr1.cc:919
    break;

  case 39:
#line 325 "qrane-parser.y" // lalr1.cc:919
    {
            yylhs.value.as < qrane_parameterlist > () = yystack_[2].value.as < qrane_parameterlist > ();
            yylhs.value.as < qrane_parameterlist > ().append(yystack_[0].value.as < std::shared_ptr<qrane_parameter> > ());
        }
#line 1237 "qrane-parser.c" // lalr1.cc:919
    break;

  case 40:
#line 331 "qrane-parser.y" // lalr1.cc:919
    { 
         yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_value<double>>(yystack_[0].value.as < double > ());
    }
#line 1245 "qrane-parser.c" // lalr1.cc:919
    break;

  case 41:
#line 335 "qrane-parser.y" // lalr1.cc:919
    { 
         yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_value<int>>(yystack_[0].value.as < unsigned int > ());
    }
#line 1253 "qrane-parser.c" // lalr1.cc:919
    break;

  case 42:
#line 339 "qrane-parser.y" // lalr1.cc:919
    { 
         yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_value<double>>(double(M_PI));
    }
#line 1261 "qrane-parser.c" // lalr1.cc:919
    break;

  case 43:
#line 343 "qrane-parser.y" // lalr1.cc:919
    { 
         yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_value<std::string>>(yystack_[0].value.as < std::string > ());
    }
#line 1269 "qrane-parser.c" // lalr1.cc:919
    break;

  case 44:
#line 347 "qrane-parser.y" // lalr1.cc:919
    { 
        yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_binaryop>(binaryop_type::ADD, yystack_[1].value.as < std::string > (), yystack_[2].value.as < std::shared_ptr<qrane_parameter> > (), yystack_[0].value.as < std::shared_ptr<qrane_parameter> > ());
    }
#line 1277 "qrane-parser.c" // lalr1.cc:919
    break;

  case 45:
#line 351 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_binaryop>(binaryop_type::SUB, yystack_[1].value.as < std::string > (), yystack_[2].value.as < std::shared_ptr<qrane_parameter> > (), yystack_[0].value.as < std::shared_ptr<qrane_parameter> > ());
    }
#line 1285 "qrane-parser.c" // lalr1.cc:919
    break;

  case 46:
#line 355 "qrane-parser.y" // lalr1.cc:919
    { 
        yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_binaryop>(binaryop_type::MUL, yystack_[1].value.as < std::string > (), yystack_[2].value.as < std::shared_ptr<qrane_parameter> > (), yystack_[0].value.as < std::shared_ptr<qrane_parameter> > ());
    }
#line 1293 "qrane-parser.c" // lalr1.cc:919
    break;

  case 47:
#line 359 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_binaryop>(binaryop_type::DIV, yystack_[1].value.as < std::string > (), yystack_[2].value.as < std::shared_ptr<qrane_parameter> > (), yystack_[0].value.as < std::shared_ptr<qrane_parameter> > ());
    }
#line 1301 "qrane-parser.c" // lalr1.cc:919
    break;

  case 48:
#line 363 "qrane-parser.y" // lalr1.cc:919
    { 
        yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_unaryop>(unaryop_type::NEG, yystack_[1].value.as < std::string > (), yystack_[0].value.as < std::shared_ptr<qrane_parameter> > ());
    }
#line 1309 "qrane-parser.c" // lalr1.cc:919
    break;

  case 49:
#line 367 "qrane-parser.y" // lalr1.cc:919
    { 
        yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_binaryop>(binaryop_type::CAR, yystack_[1].value.as < std::string > (), yystack_[2].value.as < std::shared_ptr<qrane_parameter> > (), yystack_[0].value.as < std::shared_ptr<qrane_parameter> > ());
    }
#line 1317 "qrane-parser.c" // lalr1.cc:919
    break;

  case 50:
#line 371 "qrane-parser.y" // lalr1.cc:919
    {
        yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_unaryop>(unaryop_type::PAR, "", yystack_[1].value.as < std::shared_ptr<qrane_parameter> > ());
    }
#line 1325 "qrane-parser.c" // lalr1.cc:919
    break;

  case 51:
#line 375 "qrane-parser.y" // lalr1.cc:919
    { 
        yylhs.value.as < std::shared_ptr<qrane_parameter> > () = std::make_shared<qrane_unaryop>(yystack_[3].value.as < unaryop_type > (), "", yystack_[1].value.as < std::shared_ptr<qrane_parameter> > ());
    }
#line 1333 "qrane-parser.c" // lalr1.cc:919
    break;

  case 52:
#line 379 "qrane-parser.y" // lalr1.cc:919
    { yylhs.value.as < unaryop_type > () = unaryop_type::SIN;  }
#line 1339 "qrane-parser.c" // lalr1.cc:919
    break;

  case 53:
#line 380 "qrane-parser.y" // lalr1.cc:919
    { yylhs.value.as < unaryop_type > () = unaryop_type::COS;  }
#line 1345 "qrane-parser.c" // lalr1.cc:919
    break;

  case 54:
#line 381 "qrane-parser.y" // lalr1.cc:919
    { yylhs.value.as < unaryop_type > () = unaryop_type::TAN;  }
#line 1351 "qrane-parser.c" // lalr1.cc:919
    break;

  case 55:
#line 382 "qrane-parser.y" // lalr1.cc:919
    { yylhs.value.as < unaryop_type > () = unaryop_type::EXP;  }
#line 1357 "qrane-parser.c" // lalr1.cc:919
    break;

  case 56:
#line 383 "qrane-parser.y" // lalr1.cc:919
    { yylhs.value.as < unaryop_type > () = unaryop_type::LN;   }
#line 1363 "qrane-parser.c" // lalr1.cc:919
    break;

  case 57:
#line 384 "qrane-parser.y" // lalr1.cc:919
    { yylhs.value.as < unaryop_type > () = unaryop_type::SQRT; }
#line 1369 "qrane-parser.c" // lalr1.cc:919
    break;


#line 1373 "qrane-parser.c" // lalr1.cc:919
            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  qrane_parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  // Generate an error message.
  std::string
  qrane_parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char qrane_parser::yypact_ninf_ = -48;

  const signed char qrane_parser::yytable_ninf_ = -1;

  const short
  qrane_parser::yypact_[] =
  {
       2,   -30,    34,    15,   -48,     7,    31,    54,    56,    58,
      62,    83,    83,    24,     4,     7,   -48,   -48,    18,   -48,
     -48,   -48,   119,   136,     5,    28,   137,   135,   147,   148,
     151,   149,   152,   134,    41,   153,   -48,   -48,   -48,    22,
     139,   140,    70,   -48,    17,    95,    76,   141,   -48,   144,
     145,   146,    83,   -48,   159,   109,    62,   109,   -48,   -48,
     -48,   -48,   -48,   -48,   -48,   -48,   -48,   -48,    48,    90,
     158,   -48,   -48,   -48,   156,   157,   150,    67,   -48,   154,
     150,    71,   -48,   164,   167,   169,   170,   171,   155,   133,
     173,    -4,    62,   109,   109,   109,   109,   109,   109,   109,
     174,   175,    49,   150,   -48,    91,   150,   -48,   160,   161,
     -48,   -48,   176,   -48,   -48,   179,    90,   124,    -4,    94,
     177,   -48,   138,   -48,   -48,   -48,    73,   -48,   115,   178,
     182,    52,   -48,   -48,   -48,   -48,   -48,   -48,   -48
  };

  const unsigned char
  qrane_parser::yydefact_[] =
  {
       0,     0,     0,     0,     1,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     2,     3,     5,     0,     6,
       7,    16,     0,     0,     0,     0,    30,     0,    28,    29,
      36,     0,     0,     0,     0,     0,     4,    10,    26,     0,
       0,     0,     0,    30,     0,     0,     0,     0,    19,     0,
       0,     0,     0,    18,     0,     0,     0,     0,    42,    52,
      53,    54,    55,    56,    57,    43,    40,    41,     0,    38,
       0,    23,     9,    27,     0,     0,     0,     0,    11,     0,
       0,     0,    20,     0,    31,    33,     0,     0,     0,     0,
       0,    48,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,     0,     0,    32,     0,     0,
      37,    17,     0,    50,    24,     0,    39,    44,    45,    46,
      47,    49,     0,    14,    15,    12,     0,    21,     0,     0,
       0,     0,    25,    51,    13,    22,    35,    34,     8
  };

  const short
  qrane_parser::yypgoto_[] =
  {
     -48,   -48,   -48,   187,   -48,   -48,   -48,   -18,   -48,   -48,
     -11,   -23,   -48,    -8,   -36,   -47,   -48
  };

  const signed char
  qrane_parser::yydefgoto_[] =
  {
      -1,     2,    15,    16,    17,    18,    19,    20,    21,    39,
      27,    28,    29,    31,    68,    69,    70
  };

  const unsigned char
  qrane_parser::yytable_[] =
  {
      38,    44,    46,    35,    32,     1,    77,     3,    89,    81,
      91,     6,     7,     8,     9,    10,    11,    12,    34,    42,
      13,    73,    96,    97,    98,     9,    10,    11,    12,     9,
      10,    11,    12,    78,     4,    37,     5,    79,    33,    72,
      26,    43,    45,    14,    87,    90,   116,   117,   118,   119,
     120,   121,   122,   102,    14,    55,    56,   105,    14,     9,
      10,    11,    12,    92,    43,   125,    57,    22,    93,    79,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
     126,   115,   103,   128,    55,    76,   106,    93,    14,   134,
      23,    93,    24,    79,    25,    57,    79,    82,    26,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    55,
      80,    79,   127,   138,    94,    95,    96,    97,    98,    30,
      57,    97,    98,    55,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    57,    79,   135,    40,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   113,    95,
      96,    97,    98,   133,    41,    47,    48,    94,    95,    96,
      97,    98,    94,    95,    96,    97,    98,    49,    50,    51,
      54,    52,    99,    53,    71,   100,   101,    74,    75,    83,
      84,    85,    88,   107,    86,   108,    43,   109,     0,   110,
     104,   131,   111,   112,   114,   123,   124,   136,   129,   130,
     132,   137,    36,     0,     0,    98
  };

  const short
  qrane_parser::yycheck_[] =
  {
      18,    24,    25,    14,    12,     3,    42,    37,    55,    45,
      57,     4,     5,     6,     7,     8,     9,    10,    14,    14,
      13,    39,    26,    27,    28,     7,     8,     9,    10,     7,
       8,     9,    10,    16,     0,    17,    21,    20,    14,    17,
      36,    36,    14,    36,    52,    56,    93,    94,    95,    96,
      97,    98,    99,    76,    36,    14,    15,    80,    36,     7,
       8,     9,    10,    15,    36,    16,    25,    36,    20,    20,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
     103,    92,    15,   106,    14,    15,    15,    20,    36,    16,
      36,    20,    36,    20,    36,    25,    20,    21,    36,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    14,
      15,    20,    21,   131,    24,    25,    26,    27,    28,    36,
      25,    27,    28,    14,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    25,    20,    21,    18,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    15,    25,
      26,    27,    28,    15,    18,    18,    21,    24,    25,    26,
      27,    28,    24,    25,    26,    27,    28,    20,    20,    18,
      36,    22,    14,    21,    21,    19,    19,    38,    38,    38,
      36,    36,    23,    19,    38,    18,    36,    18,    -1,    19,
      36,    15,    21,    38,    21,    21,    21,    19,    38,    38,
      21,    19,    15,    -1,    -1,    28
  };

  const unsigned char
  qrane_parser::yystos_[] =
  {
       0,     3,    40,    37,     0,    21,     4,     5,     6,     7,
       8,     9,    10,    13,    36,    41,    42,    43,    44,    45,
      46,    47,    36,    36,    36,    36,    36,    49,    50,    51,
      36,    52,    52,    14,    14,    49,    42,    17,    46,    48,
      18,    18,    14,    36,    50,    14,    50,    18,    21,    20,
      20,    18,    22,    21,    36,    14,    15,    25,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    53,    54,
      55,    21,    17,    46,    38,    38,    15,    53,    16,    20,
      15,    53,    21,    38,    36,    36,    38,    52,    23,    54,
      49,    54,    15,    20,    24,    25,    26,    27,    28,    14,
      19,    19,    50,    15,    36,    50,    15,    19,    18,    18,
      19,    21,    38,    15,    21,    49,    54,    54,    54,    54,
      54,    54,    54,    21,    21,    16,    50,    21,    50,    38,
      38,    15,    21,    15,    16,    21,    19,    19,    46
  };

  const unsigned char
  qrane_parser::yyr1_[] =
  {
       0,    39,    40,    41,    41,    42,    42,    42,    42,    43,
      43,    44,    44,    44,    45,    45,    46,    46,    46,    46,
      46,    46,    46,    47,    47,    47,    48,    48,    49,    49,
      50,    50,    51,    51,    51,    51,    52,    52,    53,    53,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    55,    55,    55,    55,    55,    55
  };

  const unsigned char
  qrane_parser::yyr2_[] =
  {
       0,     2,     4,     1,     2,     1,     1,     1,     7,     3,
       2,     4,     6,     7,     6,     6,     1,     5,     3,     3,
       4,     6,     7,     3,     5,     6,     1,     2,     1,     1,
       1,     3,     4,     3,     6,     6,     1,     4,     1,     3,
       1,     1,     1,     1,     3,     3,     3,     3,     2,     3,
       3,     4,     1,     1,     1,     1,     1,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const qrane_parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "T_OPENQASM", "T_QREG",
  "T_CREG", "T_GATE_CUSTOM", "T_OPAQUE", "T_BARRIER", "T_MEASURE",
  "T_RESET", "T_U", "T_CX", "T_IF", "\"(\"", "\")\"", "\"{\"", "\"}\"",
  "\"[\"", "\"]\"", "\",\"", "\";\"", "\"->\"", "\"==\"", "T_ADD", "T_SUB",
  "T_MUL", "T_DIV", "T_CARROT", "T_PI", "T_SIN", "T_COS", "T_TAN", "T_EXP",
  "T_LN", "T_SQRT", "T_ID", "T_REAL", "T_UINTEGER", "$accept",
  "mainprogram", "program", "statement", "decl", "gatedecl", "reg", "qop",
  "uop", "goplist", "anylist", "idlist", "mixedlist", "argument",
  "explist", "exp", "unaryop", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short
  qrane_parser::yyrline_[] =
  {
       0,   115,   115,   120,   125,   131,   135,   139,   143,   149,
     154,   159,   165,   171,   178,   186,   193,   197,   204,   211,
     219,   226,   233,   241,   249,   256,   264,   269,   275,   276,
     278,   283,   289,   294,   299,   304,   310,   314,   319,   324,
     330,   334,   338,   342,   346,   350,   354,   358,   362,   366,
     370,   374,   379,   380,   381,   382,   383,   384
  };

  // Print the state stack on the debug stream.
  void
  qrane_parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  qrane_parser::yy_reduce_print_ (int yyrule)
  {
    unsigned yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG



} // yy
#line 1854 "qrane-parser.c" // lalr1.cc:1242
#line 386 "qrane-parser.y" // lalr1.cc:1243


void yy::qrane_parser::error (const yy::location & l, const std::string & s) {
	std::cerr << "Qrane parsing error at " << l << " : " << s << std::endl;
}
