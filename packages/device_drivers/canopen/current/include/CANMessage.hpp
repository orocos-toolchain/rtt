#ifndef CANMESSAGE_HPP
#define CANMESSAGE_HPP

#include <can/cpmacro.h>
#include "CANDeviceInterface.hpp"

#define fillData(data, d0, d1, d2 ,d3, d4, d5, d6, d7) do { \
        data[0] = d0; data[1] = d1; data[2] = d2; data[3] = d3; \
        data[4] = d4; data[5] = d5; data[6] = d6; data[7] = d7; } while(0)

namespace CAN
{

    typedef ::CpStruct_CAN CANBase;

    inline unsigned int lower_u32(const unsigned char* data)
    {
        return *(unsigned int*)(data);
    }
        
    inline unsigned int higher_u32(const unsigned char* data)
    {
        return *(unsigned int*)(data + 4);
    }
        
	/**
	 * A CAN message containing message sender and message data
	 */
	struct CANMessage 
		: public CANBase
	{
		/**
		 * The type for message ids.
		 */
		typedef unsigned int ID;
		/**
		 * The type for the message flags.
		 */
		typedef unsigned int Flags;
		/**
		 * The type for the message Data.
		 */
		typedef unsigned char Data;

        /**
         * Create an empty CANMessage
         */
        CANMessage() : origin(0) { clear(); }

        /**
         * Create an empty CANMessage
         */
        CANMessage(CANDeviceInterface* _origin) : origin(_origin) { clear(); }
        
		/**
		 * Create a Standard CANMessage with default flags.
		 *
		 * @param _origin The CAN device which created this CANMessage.
		 * @param _msgid  The ID of the message.
		 * @param _data   A pointer to the data to be used (will be copied).
		 * @param _length The length of the data, the number of items in the data array (max 8)
		 */
		CANMessage(CANDeviceInterface* _origin, ID _msgid, Data* _data, unsigned int _length)
			: origin(_origin)
        {
            clear();
            setDLC(_length);
            setStdId(_msgid);
            for (unsigned int i=0; _data !=0 && i<_length; ++i)
                setData(i,_data[i]);
        }

        static CANMessage* createExtended(CANDeviceInterface* _origin, ID _msgid, Data* _data, unsigned int _length) 
        { 
            CANMessage* cm = new CANMessage(_origin, _msgid, _data, _length); 
            cm->setExtId(_msgid);
            return cm;
        }

        static CANMessage* createStandard(CANDeviceInterface* _origin, ID _msgid, Data* _data, unsigned int _length) 
        {
            return new CANMessage(_origin, _msgid, _data, _length); 
        }

        static CANMessage* createStdRemote(CANDeviceInterface* _origin, ID _msgid, Data* _data, unsigned int _length) 
        {
            CANMessage* cm= new CANMessage(_origin, _msgid, _data, _length); 
            cm->setRemote();
            return cm;
        }

        static CANMessage* createExtRemote(CANDeviceInterface* _origin, ID _msgid, Data* _data, unsigned int _length) 
        {
            CANMessage* cm= new CANMessage(_origin, _msgid, _data, _length); 
            cm->setExtId(_msgid);
            cm->setRemote();
            return cm;
        }


        void clear() { CpMacMsgClear(this); }
        
        bool isRemote() const { return CpMacIsRemote(this); }
        
        void setRemote() { CpMacSetRemote(this); }

        Data getData(unsigned int pos) const { return CpMacGetData(this,pos); }

        void setData(unsigned int pos, Data d) { CpMacSetData(this,pos,d); } 

        void setDataDLC(Data* _data, unsigned int _length) 
        { 
            CpMacSetDlc(this, _length); 
            for (unsigned int i=0; _data !=0 && i<_length; ++i)
                setData(i,_data[i]);
        }
        
        unsigned int getDLC() const { return CpMacGetDlc(this); }

        void setDLC(unsigned int length) { CpMacSetDlc(this,length); }

        bool isExtended() const { return CpMacIsExtended(this); }

        bool isStandard() const { return !isExtended(); }

        unsigned int getStdId() const { return CpMacGetStdId(this); }

        unsigned int getExtId() const { return CpMacGetExtId(this); }
		
        void setStdId(unsigned int id) { CpMacSetStdId(this,id); }
        
        void setExtId(unsigned int id) { CpMacSetExtId(this,id); }

        bool operator==(CANMessage& other) const
        {
            if ( isStandard() == other.isStandard() && ( getStdId() == other.getStdId() ) && ( getDLC() == other.getDLC() ) )
                {
                    for (unsigned int i=0; i < getDLC(); ++i)
                        if ( getData(i) != other.getData(i) )
                            return false;
                    return true;
                }
                            
            if ( isExtended() == other.isExtended() && ( getExtId() == other.getExtId() ) && ( getDLC() == other.getDLC() ) )
                {
                    for (unsigned int i=0; i < getDLC(); ++i)
                        if ( getData(i) != other.getData(i) )
                            return false;
                    return true;
                }
            return false;
        }
                            
        CANMessage& operator=(const CpStruct_CAN &msg)
        {
            v_MsgId = msg.v_MsgId;
            v_MsgFlags = msg.v_MsgFlags;
            strncpy((char*)(v_MsgData), (char*)(msg.v_MsgData),8);
            return *this;
        }
                        
        
        /**
		 * The sender of this message.
		 */
		CANDeviceInterface *origin;
	};
			
}


#endif
