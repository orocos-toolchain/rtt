/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  PropertyDecomposition.cpp

                        PropertyDecomposition.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


/**
 * PropertyDecomposition.cpp
 *
 *  Created on: May 10, 2010
 *      Author: kaltan
 */

#include "PropertyDecomposition.hpp"
#include "../internal/DataSource.hpp"
#include <vector>
#include <string>
#include <memory>
#include <boost/config.hpp>
#include <boost/lexical_cast.hpp>
#include "../Logger.hpp"
#include "TypeInfo.hpp"
#include "../Property.hpp"


using namespace std;
using namespace RTT;
using namespace RTT::detail;

namespace RTT { namespace types {

bool propertyDecomposition( base::PropertyBase* source, PropertyBag& targetbag, bool recurse )
{
    if (!source)
        return false;
    DataSourceBase::shared_ptr dsb = source->getDataSource();
    if (!dsb)
        return false;
    return typeDecomposition( dsb, targetbag, recurse);
}

bool typeDecomposition( base::DataSourceBase::shared_ptr dsb, PropertyBag& targetbag, bool recurse)
{
    if (!dsb)
        return false;

    // try user's custom type decomposition first:
    DataSourceBase::shared_ptr decomposed = dsb->getTypeInfo()->decomposeType(dsb);
    // In cases where decomposeType() returned dsb itself, we stop the decomposition here.
    if (decomposed == dsb)
        return false;
    if (decomposed) {
        // decomposed is or another type, or a PropertyBag
        internal::AssignableDataSource<PropertyBag>::shared_ptr bag = internal::AssignableDataSource<PropertyBag>::narrow( decomposed.get() );
        if ( bag ) {
            // get it and copy it.
            targetbag = bag->rvalue();
            return true;
        } else {
            // it converted to something else than a bag.
            //log(Debug) << "propertyDecomposition: decomposeType() of "<<  dsb->getTypeName() << " did not return a PropertyBag but a " << decomposed->getTypeName() << endlog();
            return false;
        }
    }

    vector<string> parts = dsb->getMemberNames();
    if ( parts.empty() ) {
        log(Debug) << "propertyDecomposition: "<<  dsb->getTypeName() << " does not have any members." << endlog();
        return false;
    }

    targetbag.setType( dsb->getTypeName() );

    // needed for recursion.
#if __cplusplus > 199711L
    unique_ptr< Property<PropertyBag> >
#else
    auto_ptr< Property<PropertyBag> >
#endif
            recurse_bag( new Property<PropertyBag>("recurse_bag","Part") );
    // First at the explicitly listed parts:
    for(vector<string>::iterator it = parts.begin(); it != parts.end(); ++it ) {
        DataSourceBase::shared_ptr part = dsb->getMember( *it );
        if (!part) {
            log(Error) <<"propertyDecomposition: Inconsistent type info for "<< dsb->getTypeName() << ": reported to have part '"<<*it<<"' but failed to return it."<<endlog();
            continue;
        }
        if ( !part->isAssignable() ) {
            // For example: the case for size() and capacity() in SequenceTypeInfo
            log(Debug)<<"propertyDecomposition: Part "<< *it << ":"<< part->getTypeName() << " is not changeable."<<endlog();
            continue;
        }
        // finally recurse or add it to the target bag:
        PropertyBase* newpb = part->getTypeInfo()->buildProperty(*it,"Part",part);
        if ( !newpb ) {
            log(Error)<< "Decomposition failed because Part '"<<*it<<"' is not known to type system."<<endlog();
            continue;
        }
        if ( !recurse )
            targetbag.ownProperty( newpb ); // leaf
        else if ( !propertyDecomposition( newpb, recurse_bag->value(), true) ) {
            //recurse_bag is empty
            assert( recurse_bag->rvalue().empty() );
            base::DataSourceBase::shared_ptr clone = newpb->getDataSource();
            base::DataSourceBase::shared_ptr converted = clone->getTypeInfo()->decomposeType(clone);
            if ( converted && converted != clone ) {
                // converted contains another type
                targetbag.add( converted->getTypeInfo()->buildProperty(*it, "", converted) );
            } else {
                // use the original
                targetbag.add( newpb );
            }
        } else {
            assert( recurse_bag->rvalue().size() >= 1 ); 
            recurse_bag->setName(*it);
            // setType() is done by recursive of self.
            targetbag.ownProperty( recurse_bag.release() ); //recursed.
            recurse_bag.reset( new Property<PropertyBag>("recurse_bag","Part") );
            delete newpb; // since we recursed, the recurse_bag now 'embodies' newpb.
        }
    }

    // Prepare recurse_bag for storing composite items:
    recurse_bag->setDescription("Item");

    // Next get the numbered parts:
    DataSource<int>::shared_ptr size = DataSource<int>::narrow( dsb->getMember("size").get() );
    if (size) {
        int msize = size->get();
        for (int i=0; i < msize; ++i) {
            string indx = boost::lexical_cast<string>( i );
            DataSourceBase::shared_ptr item = dsb->getMember(indx);
            if (item) {
                if ( !item->isAssignable() ) {
                    // For example: the case for size() and capacity() in SequenceTypeInfo
                    log(Warning)<<"propertyDecomposition: Item '"<< indx << "' of type "<< dsb->getTypeName() << " is not changeable."<<endlog();
                    continue;
                }
                // finally recurse or add it to the target bag:
                PropertyBase* newpb = item->getTypeInfo()->buildProperty( "Element" + indx,"Sequence Element",item);
                if ( !recurse || !propertyDecomposition( newpb, recurse_bag->value()) ) {
                    targetbag.ownProperty( newpb ); // leaf
                } else {
                    delete newpb;
                    recurse_bag->setName( "Element" + indx );
                    // setType() is done by recursive of self.
                    targetbag.ownProperty( recurse_bag.release() ); //recursed.
                    recurse_bag.reset( new Property<PropertyBag>("recurse_bag","Item") );
                }
            }
        }
    }
    if (targetbag.empty() )
        log(Debug) << "propertyDecomposition: "<<  dsb->getTypeName() << " returns an empty property bag." << endlog();
    return true;
}

}}
