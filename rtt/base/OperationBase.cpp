/*
 * OperationBase.cpp
 *
 *  Created on: Jan 15, 2010
 *      Author: kaltan
 */

#include "OperationBase.hpp"
#include "Operation.hpp"

namespace RTT
{
    namespace base
    {

        OperationBase::OperationBase(const std::string& name)
        :mname(name)
        {
            descriptions.push_back("(not documented)");
        }

        OperationBase::~OperationBase()
        {
        }

        void OperationBase::mdoc(const std::string& description) {
            descriptions[0] = description;
        }

        void OperationBase::marg(const std::string& name, const std::string& description) {
            descriptions.push_back(name);
            descriptions.push_back(description);
        }

    }

}
