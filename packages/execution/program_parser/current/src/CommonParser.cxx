#include "execution/parser-debug.hpp"
#include "execution/CommonParser.hpp"

namespace ORO_Execution {

      CommonParser::CommonParser()
      {
        // we reserve a few words
        keywords =
          "do", "until", "next", "done", "or", "and", "not", "include", "if",
          "define", "end", "then", "else", "for", "foreach", "while", "true",
          "false", "stop", "async", "time", "const", "frame", "double",
          "int", "bool", "char", "string", "vector", "double6d", "rotation", "twist",
          "wrench", "nothing", "var", "set", "let", "alias", "to", "sync",
          "return", "call";

        chset<> identchar( "a-zA-Z_0-9" );

        // an identifier is a word which can be used to identify a
        // label, or be the name of an object or method.  it is required
        // to start with a letter, followed by any number of letters,
        // numbers, dashes, underscores or letters.  The keywords we
        // reserved above are excluded..

        // if a rule is going to be used inside a lexeme_d, then it
        // needs to be of a different type..  Since identifier is used
        // both inside and outside of lexeme_d, we need two versions of
        // it.  Those are provided here: lexeme_identifier and
        // identifier..
        RULE( identifier_base, lexeme_d[ alpha_p >> *identchar ] - as_lower_d[keywords] );
        lexeme_identifier = identifier_base;
        identifier = identifier_base;

        BOOST_SPIRIT_DEBUG_RULE( identifier );
        BOOST_SPIRIT_DEBUG_RULE( lexeme_identifier );
      }
}
