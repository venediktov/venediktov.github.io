#pragma once 

#include "rtb/core/openrtb.hpp"
#include <boost/optional.hpp>
#include <algorithm>


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
    auto &encoded_vector = boost::any_cast<std::vector<boost::any> &>(value);
    decoded_vector.reserve(encoded_vector.size());
    std::transform( std::begin(encoded_vector), 
                    std::end(encoded_vector), 
                    std::back_inserter(decoded_vector), []( boost::any & a) {
                        return extractors<T>::extract(a);
                  }); 

    return decoded_vector;
}
};

//struct declarations parcial specializations
template<typename T>
struct extractors<openrtb::Impression<T>> {
static openrtb::Impression<T> extract( boost::any & value ); 
};

template<typename T>
struct extractors<boost::optional<openrtb::User<T>>> {
static openrtb::User<T> extract( boost::any & value ); 
};

template<typename T>
struct extractors<boost::optional<openrtb::Site<T>>> {
static openrtb::Site<T> extract( boost::any & value ); 
};

template<typename T>
struct extractors<openrtb::Banner<T>> {
static openrtb::Banner<T> extract( boost::any & value ); 
};

template<typename T>
struct extractors<openrtb::BidRequest<T>> {
static openrtb::BidRequest<T> extract( boost::any & value ) {
    openrtb::BidRequest<T> request;
    if ( value.empty() ) {
        return request;
    }
    auto &m = boost::any_cast<std::map<std::string , boost::any> &>(value);
    request.id   = boost::any_cast<decltype(request.id)>(m["id"]);
    request.imp  = extractors<decltype(request.imp)>::extract(m["imp"]);
    request.user = extractors<decltype(request.user)>::extract(m["user"]);
    request.site = extractors<decltype(request.site)>::extract(m["site"]);
    return request;
}
};

//extract functions implementation inline keyword ?
template<typename T>
openrtb::Impression<T> 
extractors<openrtb::Impression<T>>::extract( boost::any & value ) {
    openrtb::Impression<T> imp;
    if ( value.empty() ) {
        return imp;
    }
    return imp;
}

template<typename T>
openrtb::User<T> 
extractors<boost::optional<openrtb::User<T>>>::extract( boost::any & value ) {
    openrtb::User<T> user;
    if ( value.empty() ) {
        return user;
    }
    return user;
}

template<typename T>
openrtb::Site<T> 
extractors<boost::optional<openrtb::Site<T>>>::extract( boost::any & value ) {
    openrtb::Site<T> site;
    if ( value.empty() ) {
        return site;
    }
    return site;
}

template<typename T>
openrtb::Banner<T>
extractors<openrtb::Banner<T>>::extract( boost::any & value ) {
    openrtb::Banner<T> banner ;
    if ( value.empty() ) {
        return banner;
    }
    auto &m = boost::any_cast<std::map<std::string , boost::any> &>(value);
    banner.h = boost::any_cast<decltype(banner.h)>(m["h"]);
    banner.w = boost::any_cast<decltype(banner.w)>(m["w"]);
    return banner;
}


