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
#include <boost/lexical_cast.hpp>
#include "../Logger.hpp"
#include "TypeInfo.hpp"
#include "../Property.hpp"


using namespace std;
using namespace RTT;
using namespace RTT::detail;

namespace RTT { namespace types {

bool propertyDecomposition( base::PropertyBase* source, PropertyBag& targetbag )
{
    if (!source)
        return false;
    DataSourceBase::shared_ptr dsb = source->getDataSource();
    if (!dsb)
        return false;

    vector<string> parts = dsb->getPartNames();
    if ( parts.empty() )
        return false;

    // needed for recursion.
    auto_ptr< Property<PropertyBag> > recurse_bag( new Property<PropertyBag>("recurse_bag","Part") );
    // First at the explicitly listed parts:
    for(vector<string>::iterator it = parts.begin(); it != parts.end(); ++it ) {
        DataSourceBase::shared_ptr part = dsb->getPart( *it );
        if (!part) {
            log(Error) <<"propertyDecomposition: Inconsistent type info for "<< part->getTypeName() << ": reported to have part '"<<*it<<"' but failed to return it."<<endlog();
            return false;
        }
        DataSourceBase::shared_ptr aspart = dsb->getTypeInfo()->getAssignable( part );
        if (!aspart) {
            // For example: the case for size() and capacity() in SequenceTypeInfo
            log(Warning)<<"propertyDecomposition: Part '"<< *it << "' of type "<< part->getTypeName() << " is not changeable."<<endlog();
            continue;
        }
        // finally recurse or add it to the target bag:
        PropertyBase* newpb = part->getTypeInfo()->buildProperty(*it,"Part",aspart);
        if (!propertyDecomposition( newpb, recurse_bag->value()) ) {
            targetbag.ownProperty( newpb ); // leaf
        } else {
            recurse_bag->setName(*it);
            targetbag.ownProperty( recurse_bag.release() ); //recursed.
            recurse_bag.reset( new Property<PropertyBag>("recurse_bag","Part") );
        }
    }

    // Prepare recurse_bag for storing composite items:
    recurse_bag->setDescription("Item");

    // Next get the numbered parts:
    DataSource<int>::shared_ptr size = DataSource<int>::narrow( dsb->getPart("size").get() );
    if (size) {
        int msize = size->get();
        for (int i=0; i < msize; ++i) {
            string indx = boost::lexical_cast<string>( i );
            DataSourceBase::shared_ptr item = dsb->getPart(indx);
            if (item) {
                DataSourceBase::shared_ptr asitem = item->getTypeInfo()->getAssignable( item );
                if (!asitem) {
                    // For example: the case for size() and capacity() in SequenceTypeInfo
                    log(Warning)<<"propertyDecomposition: Item '"<< indx << "' of type "<< dsb->getTypeName() << " is not changeable."<<endlog();
                    continue;
                }
                // finally recurse or add it to the target bag:
                PropertyBase* newpb = item->getTypeInfo()->buildProperty(indx,"Item",asitem);
                if (!propertyDecomposition( newpb, recurse_bag->value()) ) {
                    targetbag.ownProperty( newpb ); // leaf
                } else {
                    recurse_bag->setName( indx );
                    targetbag.ownProperty( recurse_bag.release() ); //recursed.
                    recurse_bag.reset( new Property<PropertyBag>("recurse_bag","Item") );
                }
            }
        }
    }

    return true;
}

}}
