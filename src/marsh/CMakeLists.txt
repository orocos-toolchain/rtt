GLOBAL_ADD_INCLUDE( rtt/marsh CPFDemarshaller.hpp INIMarshaller.hpp Orocos1Demarshaller.hpp SimpleDemarshaller.hpp  
           TableHeaderMarshaller.hpp  XMLDemarshaller.hpp  XMLRPCDemarshaller.hpp CPFDTD.hpp CPFMarshaller.hpp
           MarshallerAdaptors.hpp Orocos1Marshaller.hpp SimpleMarshaller.hpp TableMarshaller.hpp  XMLMarshaller.hpp   
           XMLRPCMarshaller.hpp StreamProcessor.hpp EmptyHeaderMarshaller.hpp EmptyMarshaller.hpp TinyDemarshaller.hpp
           )
GLOBAL_ADD_SRC( src/marsh/CPFDTD.cpp )

IF (XERCES)
    GLOBAL_ADD_SRC( src/marsh/CPFDemarshaller.cpp )
ELSE (XERCES)
    GLOBAL_ADD_SRC( src/marsh/TinyDemarshaller.cpp src/marsh/tinyxml.cpp src/marsh/tinyxmlparser.cpp src/marsh/tinyxmlerror.cpp src/marsh/tinystr.cpp )
ENDIF (XERCES)
