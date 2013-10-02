#include <ds/meta.h>
#include <stdexcept>

using namespace ds;
using namespace std;

meta::~meta() {
}


bool
meta::contains(const string &key) const {
    return map_.count(key) > 0;
}

const string &
meta::get(const string &key) const {
    map_t::const_iterator i = map_.find(key);
    if (i == map_.end()) {
        throw runtime_error("no such key: " + key);
    }

    return i -> second;
}

void
meta::set(const string &key, const string &val) {
    map_[key] = val;
}

void
meta::keys(vector<string> &out) const {
    for (map_t::const_iterator i = map_.begin(); i != map_.end(); ++ i) {
        out.push_back(i ->first);
    }
}
