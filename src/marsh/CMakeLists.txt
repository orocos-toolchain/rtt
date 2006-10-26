GLOBAL_ADD_INCLUDE( march CPFDemarshaller.hpp INIMarshaller.hpp Orocos1Demarshaller.hpp SimpleDemarshaller.hpp  
           TableHeaderMarshaller.hpp  XMLDemarshaller.hpp  XMLRPCDemarshaller.hpp CPFDTD.hpp CPFMarshaller.hpp
           MarshallerAdaptors.hpp Orocos1Marshaller.hpp SimpleMarshaller.hpp TableMarshaller.hpp  XMLMarshaller.hpp   
           XMLRPCMarshaller.hpp StreamProcessor.hpp EmptyHeaderMarshaller.hpp EmptyMarshaller.hpp TinyDemarshaller.hpp)
GLOBAL_ADD_SRC( src/marsh/CPFDTD.cpp src/marsh/MarshallingAccess.cpp )

IF (XERCES)
    GLOBAL_ADD_SRC( src/marsh/CPFDemarshaller.cpp )
ELSE (XERCES)
    GLOBAL_ADD_SRC( TinyDemarshaller.cpp tinyxml.cpp tinyxmlparser.cpp tinyxmlerror.cpp tinystr.cpp )
ENDIF (XERCES)
