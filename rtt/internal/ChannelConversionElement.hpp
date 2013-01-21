#ifndef ORO_CHANNEL_CONVERSION_ELEMENT_HPP
#define ORO_CHANNEL_CONVERSION_ELEMENT_HPP

#include "../base/ChannelElement.hpp"
#include "../types/TypeInfoRepository.hpp"
#include "DataSource.hpp"
#include "../Logger.hpp"

namespace RTT { namespace internal {

	class ChannelConversionElementInterface {
	public:
		virtual base::DataSourceBase::shared_ptr getDataSource() = 0;
		virtual ~ChannelConversionElementInterface() {};
	};

	/** The part of the Conversion element connected to the OutputPort. */	
	template <typename T_In>
	class ChannelConversionElementIn :
		public ChannelConversionElementInterface,
		public base::ChannelElement<T_In>
	{
	protected:
		T_In in_storage;
		base::DataSourceBase::shared_ptr data_source;
		base::DataSourceBase::shared_ptr ref_data_source;
	public:
		ChannelConversionElementIn(ChannelConversionElementInterface* cce)
		{
			this->data_source = cce->getDataSource();
			base::ChannelElementBase::shared_ptr o(dynamic_cast<base::ChannelElementBase*>(cce));
			this->setOutput(o);
			ref_data_source = types::TypeInfoRepository::Instance()->getTypeInfo<T_In>()->buildReference(&in_storage);
		};
		/** Read with conversion */
		virtual FlowStatus read(typename base::ChannelElement<T_In>::reference_t sample, bool copy_old_data)
		{
			typename base::ChannelElement<T_In>::shared_ptr input = this->getInput();
			if (input) {
				// copy to in_storage
				FlowStatus fs = input->read(in_storage, copy_old_data);
				//log(Debug) << "ChannelConversionElementIn::read" << endlog();
				// conversion
				if (fs == NewData && ref_data_source) {
					if (data_source) {
						data_source->update(&(*(data_source->getTypeInfo()->convert(ref_data_source))));
						return fs;
					}
				}
			}
			return NoData;
		};

		/** Write with conversion.
		 * Should only occur for Remote connections. In local connection, conversion in done on reading.
		 */
		virtual bool write(typename base::ChannelElement<T_In>::param_t sample) {
			if (this->getOutput()) {
				in_storage = sample;
				if (ref_data_source && data_source) {
					data_source->update(&(*(data_source->getTypeInfo()->convert(ref_data_source))));
					base::ChannelElement<T_In>::write(sample);
					//log(Debug) << "ChannelConversionElementIn::write" << endlog();
					return true;
				}
			}
			return false;
		}

		virtual base::DataSourceBase::shared_ptr getDataSource() { return data_source; };
	};

	/** The part of the Conversion element connected to the InputPort. */
	template <typename T_Out>
	class ChannelConversionElementOut : 
		public ChannelConversionElementInterface,
		public base::ChannelElement<T_Out>
	{
	protected:
		typename DataSource<T_Out>::shared_ptr data_source;
	public:
		ChannelConversionElementOut(typename DataSource<T_Out>::shared_ptr ds) : data_source(ds) {};
		ChannelConversionElementOut(const RTT::types::TypeInfo* ti) {
			T_Out* d = new T_Out();
			data_source = boost::dynamic_pointer_cast<internal::ReferenceDataSource<T_Out> >(ti->buildReference(d));
		}

		virtual base::DataSourceBase::shared_ptr getDataSource() { return data_source; };

		virtual FlowStatus read(typename base::ChannelElement<T_Out>::reference_t sample, bool copy_old_data) {
			FlowStatus fs = base::ChannelElement<T_Out>::read(sample, copy_old_data);
			if (fs == NewData) {
				this->data_source->evaluate();
				sample = this->data_source->value();
				//log(Debug) << "ChannelConversionElementOut::read" << endlog();
			}
			return fs; 
		};

		/** Write with conversion.
		 * Should only occur for Remote connections. In local connection, conversion in done on reading.
		 */
		virtual bool write(typename base::ChannelElement<T_Out>::param_t sample) {
			if (this->getInput()) {
				// get the converted data from the ChannelConversionElementIn
				this->data_source->evaluate();
				//log(Debug) << "ChannelConversionElementOut::write" << endlog();
				// then write the result to the output channel
				if (this->getOutput())
					return this->getOutput()->write(this->data_source->value());
			}
			return false;
		}
	};

}}

#endif

