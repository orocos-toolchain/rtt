
#include "corelib/VectorComposition.hpp"

namespace ORO_CoreLib
{

  // A decomposeProperty method for decomposing a Property< vector<double> >
  // into a PropertyBag with Property<double>'s.
  // The dimension of the vector must be less than 100 if you want the
  // Property<double>'s to have a different name.
  void decomposeProperty(PropertyIntrospection *pi, const Property< std::vector<double> >& c)
  {
    Property<PropertyBag> result(c.getName(),"std::vector<double>", PropertyBag("std::vector<double>") );

    std::vector<double> vec = c.get();
    unsigned int dimension = vec.size();

    std::string data_name = "d00";

    for (unsigned int i=0; i < dimension ; i++)
    {
        result.value().add( new Property<double>(data_name,"",vec[i]) ); // Put variables in the bag

        // change last number of string
        if(i < 100)
        {
            if( data_name[2] == '9')
            {
                data_name[2] = '0';
                data_name[1] += 1;
            }
            else
            {
                data_name[2] += 1;
            }
        }
    }

    pi->introspect(result); // introspect the bag.
  }



  // A composeProperty method for composing a property of a vector<double>
  // The dimension of the vector must be less than 100.

  void composeProperty(const PropertyBag& bag, Property<std::vector<double> >& result)
  {
    PropertyBase* v_base = bag.find( result.getName() );
    Property<PropertyBag>* v_bag = dynamic_cast< Property<PropertyBag>* >( v_base );

    if (v_bag != NULL && v_bag->get().getType() == "std::vector<double>")
    {
        std::string data_name = "d00";
        Property<double>* comp;

        // cerr << "Getting dimension ... ";
        Property<double>* dim;
        dim = dynamic_cast< Property<double>* >(v_bag->get().find("dim"));
        int dimension = int (dim->get());
        // cerr << dimension << endl;

        if (dimension > 99)
        {
	  // cerr << "\033[1;33mWarning: Setting dimension of vector to maximum for properties (99) \033[0m" << endl;
            dimension = 99;
        }

        // Set dimension
        result.value().resize(dimension);

        // Get values
        for (int i = 0; i < dimension ; i++)
        {
            comp = dynamic_cast< Property<double>* >(v_bag->get().find(data_name));
            result.value()[i] = comp->get();

            // change last number of string
            if( data_name[2] == '9')
            {
                data_name[2] = '0';
                data_name[1] += 1;
            }
            else
            {
                data_name[2] += 1;
            }
        }
    }
    else
    {
      // cerr << "\033[1;33mWarning: Bag was empty! \033[0m" << endl;
    }
  }


}; // namespace ORO_CoreLib

