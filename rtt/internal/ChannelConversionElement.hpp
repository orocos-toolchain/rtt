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
				// conversion
				if (ref_data_source) {
					Logger::log(Logger::Debug) << "(ChannelConversionElementIn::read) reference data source exists" << endlog();
					if (data_source) {
						Logger::log(Logger::Debug) << "(ChannelConversionElementIn::read) data source type info:"
								<< data_source->getTypeInfo() << endlog();
						data_source->update(&(*(data_source->getTypeInfo()->convert(ref_data_source))));
						return fs;
					}
				}
			}
			return NoData;
		};
		/** Only a read() can start a conversion.
		 * No write may be done to this element.
		 */
		virtual bool write(typename base::ChannelElement<T_In>::param_t sample) { return false; }
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
		virtual base::DataSourceBase::shared_ptr getDataSource() { return data_source; };
		virtual FlowStatus read(typename base::ChannelElement<T_Out>::reference_t sample, bool copy_old_data) {
			FlowStatus fs = base::ChannelElement<T_Out>::read(sample, copy_old_data);
			if (fs == NewData) {
				this->data_source->evaluate();
				sample = this->data_source->value();
				log(Debug) << "(ChannelConversionElementOut::read) new data";
				if (data_source->getTypeInfo()->isStreamable()) data_source->getTypeInfo()->write(std::cout, data_source);
				log() << endlog();
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

