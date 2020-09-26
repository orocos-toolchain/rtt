/***************************************************************************
  tag: Peter Soetens Tue Jan 11 17:31:48 2011 +0100 PropertyComposition.cpp

                        PropertyComposition.cpp -  description
                           -------------------
    begin                : Tue Jan 11 2011
    copyright            : (C) 2011 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "rtt/types/PropertyComposition.hpp"
#include "rtt/types/PropertyDecomposition.hpp"
#include "rtt/Logger.hpp"
#include "rtt/Property.hpp"
#include "rtt/types/Types.hpp"

using namespace RTT;
using namespace RTT::detail;

bool RTT::types::composePropertyBag( PropertyBag const& sourcebag, PropertyBag& target )
{
    if ( !target.empty() ) {
        log(Error) <<"composePropertyBag: target bag must be empty."<< endlog();
        return false;
    }
    bool has_error = false;
    PropertyBag::const_iterator sit = sourcebag.begin();
    for( ; sit != sourcebag.end(); ++sit) {
        // test if it's a property bag, then we need to recurse:
        Property<PropertyBag> isbag;
        isbag = *sit; // separate assignment avoids error message !
        if ( isbag.ready() && isbag.value().getType() != "PropertyBag") {
            // typed property bag, need to compose it.
            TypeInfo* ti = Types()->type( isbag.value().getType() );
            if ( ti == 0) {
                log(Error) <<"Could not compose unknown type '" << isbag.value().getType() <<"'." <<endlog();
                has_error = true;
                continue;
            }
            PropertyBase* tgtprop = ti->buildProperty(isbag.getName(), isbag.getDescription());
            if ( ti->composeType(isbag.getDataSource(), tgtprop->getDataSource()) ) {
                log(Debug) << "Used user's composition function for " << tgtprop->getName() <<":"<<tgtprop->getType()<<endlog();
                target.ownProperty(tgtprop);
            } else {
                log(Error) <<"The type '" << isbag.value().getType() <<"' did not provide a type composition function, but I need one to compose it from a PropertyBag." <<endlog();
                delete tgtprop;
                has_error = true;
                continue;
            }
        } else {
            if ( isbag.ready() ) {
                // plain property bag, clone and recurse:
                Property<PropertyBag>* newbag = new Property<PropertyBag>(isbag.getName(), isbag.getDescription());
                if ( composePropertyBag(isbag.value(), newbag->value()) == false) {
                    delete newbag;
                    has_error = true;
                    continue;
                }
                target.ownProperty( newbag );
            } else {
                // plain property, not a bag:
                target.ownProperty( (*sit)->clone() );
            }
        }
    }

    return !has_error;
}

bool RTT::types::decomposePropertyBag( PropertyBag const& sourcebag, PropertyBag&  target)
{
    if ( !target.empty() ) {
        log(Error) <<"decomposePropertyBag: target bag must be empty."<< endlog();
        return false;
    }
    PropertyBag::const_iterator sit = sourcebag.begin();
    while( sit != sourcebag.end()) {
        // test if it's a property bag, then we need to recurse:
        Property<PropertyBag> isbag;
        isbag = *sit; // avoid logging error
        if ( isbag.ready() ) {
            // create an empty instance to recurse in:
            Property<PropertyBag>* newtarget = new Property<PropertyBag>(isbag.getName(), isbag.getDescription() );
            newtarget->value().setType( isbag.rvalue().getType() );
            target.ownProperty( newtarget );
            if ( decomposePropertyBag(isbag.value(), newtarget->value() ) == false) {
                assert(false && "internal error in decomposePropertyBag."); // this is a best effort function.
            }
        } else {
            // decompose non-bag type:
            log(Debug) << "Checking for decompose "<< (*sit)->getName() <<endlog();

            // Try decomposeType() first because this is the user's implementation of decomposition:
            DataSourceBase::shared_ptr dsb = (*sit)->getTypeInfo()->decomposeType( (*sit)->getDataSource() );
            if ( dsb ) {
                // check if type returned itself to avoid decomposition:
                if ( dsb == (*sit)->getDataSource() ) {
                    // primitive type : clone the instance
                    target.ownProperty( (*sit)->clone() );
                } else {
                    DataSource<PropertyBag>::shared_ptr bagds = DataSource<PropertyBag>::narrow(dsb.get());
                    if ( bagds ) {
                        // property bag ? -> further decompose
                        // create an empty instance to recurse in:
                        Property<PropertyBag>* newtarget = new Property<PropertyBag>((*sit)->getName(), (*sit)->getDescription() );
                        newtarget->value().setType( bagds->rvalue().getType() );
                        target.ownProperty( newtarget );
                        if ( decomposePropertyBag(bagds->rvalue(), newtarget->value() ) == false) {
                            assert(false && "internal error in decomposePropertyBag."); // this is a best effort function.
                        }
                    } else {
                        // other type ? -> add
                        base::PropertyBase* p = dsb->getTypeInfo()->buildProperty((*sit)->getName(), (*sit)->getDescription(), dsb);
                        if ( target.ownProperty( p ) == false)
                            log(Error) <<"Failed to create a property of decomposed data of type "<<(*sit)->getType() <<endlog();
                    }
                }
            } else {
                // if decomposeType() returned null, try generic decomposition, based on getMember():
                Property<PropertyBag> res((*sit)->getName(), (*sit)->getDescription() );
                // propertyDecomposition decomposes fully, so the result does not need to be recursed:
                if ( types::propertyDecomposition(*sit, res.value() ) ) {
                    target.ownProperty( res.clone() );
                }
            }
        }
        ++sit;
    }
    return true;
}
