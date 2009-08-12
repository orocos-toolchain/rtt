
GLOBAL_ADD_INCLUDE( rtt/marsh PropertyMarshaller.hpp PropertyDemarshaller.hpp)
GLOBAL_ADD_SRC( marsh/PropertyMarshaller.cpp marsh/PropertyDemarshaller.cpp marsh/PropertyIntrospection.cpp marsh/PropertyBagIntrospector.cpp)

IF (ENABLE_MARSHALLING)
  GLOBAL_ADD_INCLUDE( rtt/marsh CPFMarshaller.hpp
           XMLRPCDemarshaller.hpp XMLRPCMarshaller.hpp CPFDTD.hpp
           StreamProcessor.hpp )
  GLOBAL_ADD_SRC( marsh/CPFDTD.cpp marsh/CPFMarshaller.cpp)

  IF (XERCES_FOUND AND NOT OS_NOEXCEPTIONS)
    GLOBAL_ADD_INCLUDE( rtt/marsh CPFDemarshaller.hpp)
    GLOBAL_ADD_SRC( marsh/CPFDemarshaller.cpp )
  ELSE (XERCES_FOUND AND NOT OS_NOEXCEPTIONS)
    GLOBAL_ADD_INCLUDE( rtt/marsh TinyDemarshaller.hpp )
    GLOBAL_ADD_SRC( marsh/TinyDemarshaller.cpp marsh/tinyxml.cpp marsh/tinyxmlparser.cpp marsh/tinyxmlerror.cpp marsh/tinystr.cpp )
  ENDIF (XERCES_FOUND AND NOT OS_NOEXCEPTIONS)
ENDIF(ENABLE_MARSHALLING)
