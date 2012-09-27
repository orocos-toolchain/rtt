#include "MemberFactory.hpp"
#include "../Logger.hpp"

using namespace std;
using namespace RTT;
using namespace RTT::detail;

bool MemberFactory::resize(base::DataSourceBase::shared_ptr arg, int size) const {
    return false;
}

    vector<string> MemberFactory::getMemberNames() const
    {
        return vector<string>();
    }

    DataSourceBase::shared_ptr MemberFactory::getMember(DataSourceBase::shared_ptr item, const std::string& part_name) const
    {
        /** ** Strong typed data **
         *
         * for( set i = 0; i <=10; set i = i + 1) {
         *      value[i] = i; // sequence index, runtime structure
         *      value.i  = i; // part name, browse static structure
         * }
         * set frame.pos = vector(a,b,c);     // getMember("pos")
         * set frame.pos[3] = vector(a,b,c);  // getMember("pos")->getMember(3)
         * set frame[3].pos = vector(a,b,c);  // getMember(3)->getMember("pos")
         * set frame[i].pos = vector(a,b,c);  // getMember( $i )->getMember("pos")
         * set frame["tool"].pos = vector(a,b,c); // getMember("tool") xx
         * set frame[arg].pos = vector(a,b,c);// getMember( arg )->getMember("pos")
         */
        if ( part_name.empty() )
            return item;
        else
            return DataSourceBase::shared_ptr();
    }

    DataSourceBase::shared_ptr MemberFactory::getMember(DataSourceBase::shared_ptr item, DataSourceBase::shared_ptr id) const
    {
        /** ** Strong typed data **
         *
         * for( set i = 0; i <=10; set i = i + 1) {
         *      value[i] = i; // sequence index, runtime structure
         *      value.i  = i; // part name, browse static structure
         * }
         * set frame.pos = vector(a,b,c);     // getMember("pos")
         * set frame.pos[3] = vector(a,b,c);  // getMember("pos")->getMember(3)
         * set frame[3].pos = vector(a,b,c);  // getMember(3)->getMember("pos")
         * set frame[i].pos = vector(a,b,c);  // getMember( $i )->getMember("pos")
         * set frame["tool"].pos = vector(a,b,c); // getMember("tool") xx
         * set frame[arg].pos = vector(a,b,c);// getMember( arg )->getMember("pos")
         */
        return DataSourceBase::shared_ptr();
    }


