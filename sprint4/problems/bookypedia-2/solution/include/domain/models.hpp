#pragma once

#include <boost/uuid/uuid.hpp>
#include <string>
#include <vector>

namespace domain {

using Id = boost::uuids::uuid;

struct Author {
    Id id;
    std::string name;
};

struct Book {
    Id id;
    Id author_id;
    std::string title;
    int publication_year;
};

struct BookTag {
    Id book_id;
    std::string tag;
};

} // namespace domain
