/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ValueParser.hpp 

                        ValueParser.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef VALUEPARSER_HPP
#define VALUEPARSER_HPP

#include "parser-common.hpp"
#include "AssignVariableCommand.hpp"
#include <corelib/CommandInterface.hpp>

#include <memory>
#include <map>

namespace ORO_Execution
{
    /**
     * @brief A class for keeping track of parsed values/variables.
     *
     * It recognizes types and stores them in a map,
     * so that they can be referenced to later on.
     * If the 'context' has changed, it must be reset.
     */
  class ValueParser
  {
    rule_t constant, const_double, const_int, const_bool, named_constant,
      const_string;
    CommonParser commonparser;

    typedef std::map<const std::string, ParsedValueBase*> map_t;
    map_t values;
    // a auto_ptr used only to make sure we don't forget to delete
    // the ParsedValueBase it holds..  Here we store a pointer to
    // the ParsedValueBase for a temporary variable we've just
    // parsed.  If we parse a non-temporary, then we don't need to
    // delete it, as it remains in the values map, however a
    // constant only gets stored temporarily in ret, so this
    // variable makes sure it gets deleted.
    std::auto_ptr<ParsedValueBase> deleter;
    // the ParsedValue we've just parsed..  we only store it and
    // assume that the ProgramParser will do something useful with
    // it.  We don't ever own it, and don't delete it, or set it to
    // 0..
    ParsedValueBase* ret;
    // contains the string constant we're parsing ( it has to be
    // parsed char-by-char, because of the way c_escape_ch_p works
    // )..
    std::string mcurstring;

    void seennamedconstant( iter_t begin, iter_t end );
    void seenboolconstant( iter_t begin, iter_t end );
    void seenintconstant( int c );
    void seendoubleconstant( double c );
    void push_str_char( char c );
    void seenstring();

  public:
    ValueParser();
    ~ValueParser();
    void clear();

    rule_t& parser();

    const ParsedValueBase* lastParsed() const
      {
        return ret;
      };

    bool isDefined( const std::string& name ) const;

    void addConstant( const std::string& name, bool value );
    void addConstant( const std::string& name, double value );
    void addConstant( const std::string& name, int value );
    void addConstant( const std::string& name, const std::string& value );
    void setValue( const std::string& name, ParsedValueBase* pc );
    void removeValue( const std::string& name );
    /**
     * Get the value with name name.  If no such value exists, this
     * returns 0.
     */
    ParsedValueBase* getValue( const std::string& name );
  };
}

#endif
