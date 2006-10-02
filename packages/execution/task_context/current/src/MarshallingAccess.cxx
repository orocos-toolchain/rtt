#include "rtt/MarshallingAccess.hpp"
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
#include "rtt/Method.hpp"
#endif
#include "rtt/PropertyLoader.hpp" 

namespace RTT
{
    MarshallingAccess::MarshallingAccess(TaskContext* parent)
        : mparent(parent)
    {
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
        OperationInterface* obj = parent->getObject("marshalling");
        if (!obj)
            obj = new TaskObject("marshalling","Read and write Properties to a file.");
        obj->methods()->addMethod(method("readProperties",&MarshallingAccess::readProperties, this),
                                  "Read Properties from a file.",
                                  "Filename","The file to read the Properties from.");
        obj->methods()->addMethod(method("writeProperties",&MarshallingAccess::writeProperties, this),
                                  "Write Properties to a file.",
                                  "Filename","The file to write the Properties to.");
        mparent->addObject( obj );
#endif
    }
    bool MarshallingAccess::readProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.configure( filename, mparent, true); // strict == true
    }
    bool MarshallingAccess::writeProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.save( filename, mparent);
    }

}
