#include "MarshallingAccess.hpp"
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
#include "Method.hpp"
#endif
#include "PropertyLoader.hpp" 

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
                                  "Read some Properties from a file.",
                                  "Filename","The file to read the Properties from.");
        obj->methods()->addMethod(method("readAllProperties",&MarshallingAccess::readAllProperties, this),
                                  "Read all Properties from a file.",
                                  "Filename","The file to read the Properties from.");
        obj->methods()->addMethod(method("readProperty",&MarshallingAccess::readProperty, this),
                                  "Read a single Property from a file.",
                                  "Name", "The name of (or the path to) the property to read.",
                                  "Filename","The file to read the Properties from.");

        obj->methods()->addMethod(method("writeProperties",&MarshallingAccess::writeProperties, this),
                                  "Write some Properties to a file.",
                                  "Filename","The file to write the Properties to.");
        obj->methods()->addMethod(method("writeAllProperties",&MarshallingAccess::writeAllProperties, this),
                                  "Write all Properties to a file.",
                                  "Filename","The file to write the Properties to.");
        obj->methods()->addMethod(method("writeProperty",&MarshallingAccess::writeProperty, this),
                                  "Write a single Properties to a file.",
                                  "Name", "The name of (or the path to) the property to write.",
                                  "Filename","The file to write the Properties to.");
        mparent->addObject( obj );
#endif
    }
    bool MarshallingAccess::readProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.configure( filename, mparent, false); // not all
    }
    bool MarshallingAccess::readAllProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.configure( filename, mparent, true); // all
    }
    bool MarshallingAccess::writeProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.save( filename, mparent, false); // all
    }
    bool MarshallingAccess::writeAllProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.save( filename, mparent, true); //
    }

    bool MarshallingAccess::readProperty(const std::string& name, const std::string& filename) {
        PropertyLoader p;
        return p.configure(filename, mparent, name);
    }

    bool MarshallingAccess::writeProperty(const std::string& name, const std::string& filename) {
        PropertyLoader p;
        return p.save(filename, mparent, name);
    }


}
