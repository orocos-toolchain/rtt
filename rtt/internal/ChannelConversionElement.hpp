#ifndef ORO_CHANNEL_CONVERSION_ELEMENT_HPP
#define ORO_CHANNEL_CONVERSION_ELEMENT_HPP

#include "../base/ChannelElement.hpp"
#include "DataSource.hpp"

namespace RTT { namespace internal {

	template <typename T>
	class ChannelConversionElementInterface : public base::ChannelElement<T> {
	        /** Only a read() can start a conversion. 
		 * No write may be done to this element.
		 */
	        virtual bool write(param_t sample) { return false; }

	};

	/** The part of the Conversion element connected to the OutputPort. */	
	template <typename T_In>
	class ChannelConversionElementIn : public ChannelConversionElementInterface<T_In> {
	protected:
		DataSource<T_In> in_storage;
	public:

        virtual FlowStatus read(reference_t sample, bool copy_old_data)
        {
		base::ChannelElement<T_In>::shared_ptr input = this->getInput();
            	if (input) {
                	FlowStatus fs = input->read(in_storage, copy_old_data); // copy to in_storage
	                return fs;
		} 
		else return NoData;
        };

	};

	/** The part of the Conversion element connected to the InputPort. */
	template <typename T_Out>
	class ChannelConversionElementOut : public ChannelConversionElementInterface<T_Out> {
	};

}}

#endif

