
/* 
 * File:   openrtb_encoders.hpp
 * Author: vladimir venediktov
 *
 * Created on April 9, 2017, 7:39 PM
 */

#ifndef JSONV_STORE_HPP
#define JSONV_STORE_HPP

#include "parsers/jsmn.h"
#include "rtb/common/string_view_to_number.hpp"
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/string_view.hpp>
#include <vector>


#include "core/openrtb.hpp"

namespace  encoders {
    
static inline
int encode(const char *js, jsmntok_t *t, size_t count, boost::any &value) {
    int i, j;
    if (count == 0) {
        return 0;
    }
    if (t->type == JSMN_PRIMITIVE) {
        auto s = boost::string_view(js + t->start, t->end - t->start);
        if (s.find_first_of('.') != boost::string_view::npos)
            value = boost::lexical_cast<double>(s.data(), s.size());
        else if (!s.empty() && s[0] == '-')
            value = vanilla::common::strtol(s); //value = boost::lexical_cast<std::int64_t>(s.data(), s.size());
        else if ( !s.empty() && s[0] == 't' )
            value = true;
        else if ( !s.empty() && s[0] == 'f' )
            value = false;
        else
            value = static_cast<int64_t>(boost::lexical_cast<uint64_t>(s.data(), s.size()));
        return 1;
    } else if (t->type == JSMN_STRING) {
        value = boost::string_view(js + t->start, t->end - t->start);
        return 1;
    } else if (t->type == JSMN_OBJECT) {
        value = std::map<boost::string_view, boost::any>();
        j = 0;
        for (i = 0; i < t->size; ++i) {
            boost::any k;
            boost::any v;
            j += encode(js, t+1+j, count-j, k);
            j += encode(js, t+1+j, count-j, v);
            if ( k.empty() || v.empty()) {
                continue;
            }
            boost::any_cast<std::map<boost::string_view, boost::any>>(value).insert({std::move(boost::any_cast<boost::string_view>(k)),std::move(v)});
        }
        return j + 1;
    } else if (t->type == JSMN_ARRAY) {
        j = 0;
        value = std::vector<boost::any>();
        boost::any_cast<std::vector<boost::any>>(value).reserve(t->size);
        for (i = 0; i < t->size; ++i) {
            boost::any v;
            j += encode(js, t+1+j, count-j, v);
            if ( !v.empty()) {
                boost::any_cast<std::vector<boost::any>>(value).push_back(std::move(v));
            }
        }
        return j + 1;
    }
    return 0;
}
} //namespace



using namespace openrtb;

template<typename T1, typename ...T2>
struct extractors {
}; 

template<typename T>
struct extractors<BidRequest<T>,T> {
static BidRequest<T> extract( boost::any & value ) {
    BidRequest<T> request;
    if ( value.empty() ) {
        return request;
    }
    //auto m = boost::any_cast<std::map<std::string , boost::any>>(value);
    //request.id   = boost::any_cast<decltype(request.id)>(m["id"]);
    //request.imp  = extract<Impression<T>>(m["imp"]);
    //request.user = extract<User<T>>(m["user"]);
    //request.site = extract<Site<T>>(m["site"]);
    return request;
}
};

/*******************
template<typename T>
Impression<T> extract<Impression<T>>( boost::any & value ) {
    Impression<T> imp;
    if ( value.empty() ) {
        return imp;
    }
    return imp;
}

template<typename T>
User<T> extract( boost::any & value ) {
    User<T> user;
    if ( value.empty() ) {
        return user;
    }
    return user;
}

template<typename T>
Site<T> extract( boost::any & value ) {
    Site<T> site;
    if ( value.empty() ) {
        return site;
    }
    return site;
}

template<typename T>
Banner<T> extract( boost::any & value ) {
    Banner<T> banner ;
    if ( value.empty() ) {
        return banner;
    }
    auto m = boost::any_cast<std::map<std::string , boost::any>>(value);
    banner.h = boost::any_cast<decltype(banner.h)>(m["h"]);
    banner.w = boost::any_cast<decltype(banner.w)>(m["w"]);
    return banner;
}

******************************/
/********************************************
            formats base_in = formats_builder()
                .type<Banner>()
                .member("h", &Banner::h)
                .member("w", &Banner::w)
                .member("pos", &Banner::pos)
                .template enum_type<AdPosition>("pos",
                {
                    { AdPosition::UNKNOWN,  0 },
                    { AdPosition::ABOVE, 1 },
                    { AdPosition::BETWEEN_DEPRECATED, 2 },
                    { AdPosition::BELOW, 3 },
                    { AdPosition::HEADER, 4 },
                    { AdPosition::FOOTER, 5 },
                    { AdPosition::SIDEBAR, 6 },
                    { AdPosition::FULLSCREEN,7 }
                })
                .template type<Impression>()
                .member("id", &Impression::id)
                .member("banner", &Impression::banner)
                .member("bidfloor", &Impression::bidfloor)
                .member("bidfloorcur", &Impression::bidfloorcur)
                .template type<User>()
                    .member("id", &User::id)
                    .member("buyeruid", &User::buyeruid)
                    .member("geo", &User::geo)
                .template type<Geo>()
                    .member("city", &Geo::city)
                    .member("country", &Geo::country)
                .template type<Site>()
                .member("id", &Site::id)
                .template type<Publisher>()
                .template type<BidRequest>()
                .member("id", &BidRequest::id)
                .member("imp", &BidRequest::imp)
                .member("user", &BidRequest::user)
                .member("site", &BidRequest::site)
                .encode_if([](const jsonv::serialization_context&, const boost::optional<Site>& x) {return bool(x);})
                .template register_container<std::vector<Impression>>()
                .template register_optional<boost::optional<Banner>>()
                .template register_optional<boost::optional<Site>>()
                .template register_optional<boost::optional<Publisher>>()
                .template register_optional<boost::optional<User>>()
                .template register_optional<boost::optional<Geo>>()
                .template register_container<std::vector<T>>()
                .template register_container<std::vector<int>>()
                .check_references(formats::defaults())
                ;

                return formats::compose({ base_in, formats::defaults() });
         }
************************************************************************************/


#endif /* JSONV_STORE_HPP */

