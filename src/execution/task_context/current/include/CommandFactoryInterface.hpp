/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  CommandFactoryInterface.hpp 

                        CommandFactoryInterface.hpp -  description
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
 
#ifndef COMMANDFACTORYINTERFACE_HPP
#define COMMANDFACTORYINTERFACE_HPP

#include <string>
#include <vector>
#include <utility>
#include "FactoryExceptions.hpp"
#include "ArgumentDescription.hpp"
#include "DispatchInterface.hpp"
#include "PropertyBag.hpp"
#include "CommandInterface.hpp"
#include "ConditionInterface.hpp"

namespace RTT
{
    class CommandProcessor;
    typedef std::pair<CommandInterface*, ConditionInterface*> ComCon;

    /**
     * @brief An interface for creating CommandInterface instances.
     *
     * Basically, this class is something which you can ask "give me
     * an object that will call a method by a certain name ( that you
     * pass as a string ), with these arguments ( that you pass in one
     * of two ways, see below )".  There is a generic implementation
     * of this in TemplateCommandFactory.hpp, you are encouraged to
     * use that one, as this interface has some rather difficult
     * requirements..
     *
     * Some of these methods throw exceptions that you can find in
     * FactoryExceptions.hpp
     */
    class CommandFactoryInterface
    {
    protected:
        CommandProcessor* mcp;
    public:
        typedef std::vector< ArgumentDescription > Descriptions;
        typedef std::vector<std::string> Commands;
        typedef std::vector<DataSourceBase::shared_ptr> Arguments;

        virtual ~CommandFactoryInterface();

        void setProcessor( CommandProcessor* cp ) { mcp = cp; }
  
        /**
         * @brief Return whether the command com is available..
         * throws nothing.
         */
        virtual bool hasCommand(const std::string& com) const = 0;

        /**
         * @brief Return a list of all available commands.
         * throws nothing.
         */
        virtual std::vector<std::string> getCommandList() const = 0;

        /**
         * Returns the arity (number of arguments) of this command.
         * @retval -1 When the command does not exist.
         */
        virtual int getArity(const std::string& com ) const = 0;

        /**
         * @brief Return the result type of a given command.
         * @param com The command to get the description from.
         */
        virtual std::string getResultType( const std::string& com ) const = 0;

        /**
         * @brief Return the description of a given command.
         * @param com The command to get the description from.
         */
        virtual std::string getDescription( const std::string& com ) const = 0;

        /**
         * @brief Used to find out what types of arguments
         * a command needs.
         *
         * This method returns a PropertyBag
         * containing Property's of the types of the arguments..
         * The user can then fill up this PropertyBag, and
         * construct a command with it via the create method
         * below.
         *
         * Note that the Properties in the property bag have been
         * constructed using new, and the caller gains their
         * ownership.  You should explicitly call
         * deleteProperties on the bag after you're done with it.
         * Store the PropertyBag in a PropertyBagOwner if you're
         * affraid you'll forget it.
         *
         * @throw name_not_found_exception
         */
        virtual PropertyBag
        getArgumentSpec( const std::string& command ) const = 0;

        /**
         * @brief Return the list of arguments of a certain command.
         */
        virtual std::vector< ArgumentDescription > getArgumentList( const std::string& command ) const = 0;

        /**
         * @brief The companion to getArgumentSpec().  It takes a
         * PropertyBag, containing Property's of the same type and
         * in the same order as the ones that getArgumentSpec()
         * returned, and constructs a ComCon with it.
         *
         * this does not delete the properties in bag, as stated
         * above, if you obtained the propertybag from
         * getArgumentSpec, then you're responsible to delete
         * it.
         * @throw name_not_found_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
         */
        virtual ComCon create( const std::string& command,
                               const PropertyBag& args,
                               bool asyn) const = 0;

        /**
         * @brief Create a Command passing DataSources as the arguments.
         *
         * In this case, the command factory should *not* read out
         * the DataSource's, but it should return a Command and a
         * Condition that store the DataSources, and read them out
         * in their execute() and evaluate() methods.  They should
         * of course both keep a reference to the DataSource's
         * that they store.  These are rather complicated
         * requirements, you are encouraged to look at the
         * TemplateCommandFactory or its helper classes to do
         * (some of) the work for you.
         * @throw name_not_found_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
         */
        virtual ComCon create(
                              const std::string& command,
                              const std::vector<DataSourceBase::shared_ptr>& args,
                              bool asyn ) const = 0;

        /**
         * @brief Create a Command passing DataSources as the arguments.
         *
         * In this case, the command factory should *not* read out
         * the DataSource's, but it should return a Command and a
         * Condition that store the DataSources, and read them out
         * in their execute() and evaluate() methods.  They should
         * of course both keep a reference to the DataSource's
         * that they store.  These are rather complicated
         * requirements, you are encouraged to look at the
         * TemplateCommandFactory or its helper classes to do
         * (some of) the work for you.
         * @throw name_not_found_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
         * @deprecated by create( const std::string&,const std::vector<DataSourceBase::shared_ptr>& )
         */
        virtual ComCon create(
                              const std::string& command,
                              const std::vector<DataSourceBase*>& args,
                              bool asyn ) const = 0;

    };
};

#endif
