#pragma once 

#include "rtb/core/openrtb.hpp"
#include <algorithm>

//using namespace openrtb;

template<typename T>
struct extractors {
static T extract( boost::any & value );
};

template<typename T>
struct extractors<std::vector<T>> {
static std::vector<T> extract( boost::any & value ) {
    std::vector<T> decoded_vector;
    if ( value.empty() ) {
        return decoded_vector;
    }
    auto encoded_vector = boost::any_cast<std::vector<boost::any>>(value);
    decoded_vector.reserve(encoded_vector.size());
    std::transform( std::begin(encoded_vector), 
                    std::end(encoded_vector), 
                    std::back_inserter(decoded_vector), []( boost::any & a) {
                        return extractors<T>::extract(a);
                  }); 

    return decoded_vector;
}
};

template<typename T>
struct extractors<openrtb::Impression<T>> {
static openrtb::Impression<T> extract( boost::any & value ); 
};



template<typename T>
struct extractors<openrtb::BidRequest<T>> {
static openrtb::BidRequest<T> extract( boost::any & value ) {
    openrtb::BidRequest<T> request;
    if ( value.empty() ) {
        return request;
    }
    auto m = boost::any_cast<std::map<std::string , boost::any>>(value);
    request.id   = boost::any_cast<decltype(request.id)>(m["id"]);
    request.imp  = extractors<decltype(request.imp)>::extract(m["imp"]);
    //request.user = extract<User<T>>(m["user"]);
    //request.site = extract<Site<T>>(m["site"]);
    return request;
}
};

template<typename T>
openrtb::Impression<T> 
extractors<openrtb::Impression<T>>::extract( boost::any & value ) {
    openrtb::Impression<T> imp;
    if ( value.empty() ) {
        return imp;
    }
    return imp;
}

/*******************
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
*****************/

