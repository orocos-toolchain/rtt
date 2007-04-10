
IF (ENABLE_MARSHALLING)
  GLOBAL_ADD_INCLUDE( rtt/marsh INIMarshaller.hpp Orocos1Demarshaller.hpp SimpleDemarshaller.hpp CPFMarshaller.hpp
           TableHeaderMarshaller.hpp  XMLDemarshaller.hpp  XMLRPCDemarshaller.hpp CPFDTD.hpp
           MarshallerAdaptors.hpp Orocos1Marshaller.hpp SimpleMarshaller.hpp TableMarshaller.hpp  XMLMarshaller.hpp   
           XMLRPCMarshaller.hpp StreamProcessor.hpp EmptyHeaderMarshaller.hpp EmptyMarshaller.hpp PropertyMarshaller.hpp PropertyDemarshaller.hpp
           )
  GLOBAL_ADD_SRC( marsh/CPFDTD.cpp marsh/PropertyMarshaller.cpp marsh/PropertyDemarshaller.cpp marsh/CPFMarshaller.cpp)

  IF (XERCES)
    GLOBAL_ADD_INCLUDE( rtt/marsh CPFDemarshaller.hpp)
    GLOBAL_ADD_SRC( marsh/CPFDemarshaller.cpp )
    SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -lxerces-c" CACHE INTERNAL "")
  ELSE (XERCES)
    GLOBAL_ADD_INCLUDE( rtt/marsh TinyDemarshaller.hpp )
    GLOBAL_ADD_SRC( marsh/TinyDemarshaller.cpp marsh/tinyxml.cpp marsh/tinyxmlparser.cpp marsh/tinyxmlerror.cpp marsh/tinystr.cpp )
  ENDIF (XERCES)
ENDIF(ENABLE_MARSHALLING)
