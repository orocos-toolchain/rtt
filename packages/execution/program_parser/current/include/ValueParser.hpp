/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ValueParser.hpp 

                        ValueParser.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
#ifndef VALUEPARSER_HPP
#define VALUEPARSER_HPP

#include "parser-types.hpp"
#include "CommonParser.hpp"
#include "PeerParser.hpp"
#include "AttributeRepository.hpp"

#include <memory>
#include <map>

namespace ORO_Execution
{
    class TaskContext;

    /**
     * @brief A class for parsing const values.
     *
     * It recognizes types and stores them in a TaskVariableBase.
     */
  class ValueParser
  {
    rule_t constant, const_double, const_int, const_bool, named_constant,
      const_string, const_char;
    CommonParser commonparser;
    PeerParser peerparser;

    // a auto_ptr used only to make sure we don't forget to delete
    // the TaskVariableBase it holds..  Here we store a pointer to
    // the TaskVariableBase for a temporary variable we've just
    // parsed.  If we parse a non-temporary, then we don't need to
    // delete it, as it remains in the values map, however a
    // constant only gets stored temporarily in ret, so this
    // variable makes sure it gets deleted.
    std::auto_ptr<TaskAttributeBase> deleter;
    // the TaskVariable we've just parsed..  we only store it and
    // assume that the ProgramParser will do something useful with
    // it.  We don't ever own it, and don't delete it, or set it to
    // 0..
    TaskAttributeBase* ret;
    // contains the string constant we're parsing ( it has to be
    // parsed char-by-char, because of the way c_escape_ch_p works
    // )..
    std::string mcurstring;

    void seennamedconstant( iter_t begin, iter_t end );
    void seenboolconstant( iter_t begin, iter_t end );
    void seenintconstant( int c );
    void seencharconstant( iter_t c );
    void seendoubleconstant( double c );
    void push_str_char( char c );
    void seenstring();

      TaskContext* context;
  public:
    ValueParser( TaskContext* tc );

      void setContext( TaskContext* tc);
      void setStack( TaskContext* tc );

    ~ValueParser();
      /**
       * Clears this parser, not the repository where it stores its results.
       */
    void clear();

    rule_t& parser();

    const TaskAttributeBase* lastParsed() const
      {
        return ret;
      }
  };
}

#endif
