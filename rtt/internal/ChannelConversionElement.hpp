#ifndef ORO_CHANNEL_CONVERSION_ELEMENT_HPP
#define ORO_CHANNEL_CONVERSION_ELEMENT_HPP

#include "../base/ChannelElement.hpp"
#include "DataSource.hpp"

namespace RTT { namespace internal {

	class ChannelConversionElementInterface {
	protected:
		base::DataSourceBase::shared_ptr data_source;
	};

	/** The part of the Conversion element connected to the OutputPort. */	
	template <typename T_In>
	class ChannelConversionElementIn : 
		public ChannelConversionElementInterface, 
		public base::ChannelElement<T_In>
	{
	protected:
		DataSource<T_In> in_storage;
	public:
		ChannelConversionElementIn(ChannelConversionElementInterface& cce) {
			this->data_source->set(cce.data_source);
			this->setOutput(cce);
		};
	        virtual FlowStatus read(typename base::ChannelElement<T_In>::reference_t sample, bool copy_old_data)
	        {
			typename base::ChannelElement<T_In>::shared_ptr input = this->getInput();
	            	if (input) {
				// copy to in_storage
	                	FlowStatus fs = input->read(in_storage, copy_old_data); 
				// conversion
				data_source = data_source->getTypeInfo()->convert(in_storage);
		                return fs;
			} 
			else return NoData;
	        };
	        /** Only a read() can start a conversion. 
		 * No write may be done to this element.
		 */
	        virtual bool write(typename base::ChannelElement<T_In>::param_t sample) { return false; }
	};

	/** The part of the Conversion element connected to the InputPort. */
	template <typename T_Out>
	class ChannelConversionElementOut : 
		public ChannelConversionElementInterface, 
		public base::ChannelElement<T_Out>
	{
	public:
		ChannelConversionElementOut(base::DataSourceBase& ds) {
			this->data_source->set(ds);
		};
		virtual FlowStatus read(typename base::ChannelElement<T_Out>::reference_t sample, bool copy_old_data) {
			FlowStatus fs = base::ChannelElement<T_Out>::read(sample, copy_old_data);
			if (fs == NewData) {
				data_source->evaluate();
				sample = data_source;
			}
			return fs; 
		};
	        /** Only a read() can start a conversion. 
		 * No write may be done to this element.
		 */
	        virtual bool write(typename base::ChannelElement<T_Out>::param_t sample) { return false; }
	};

}}

#endif

