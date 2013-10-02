#ifndef __DS_META_H__
#define __DS_META_H__
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace ds {
    class meta {
    private:
        typedef map<string, string> map_t;
        map_t map_;

    public:
        virtual ~meta();

        bool contains(const string &key) const;
        const string & get(const string &key) const;
        void set(const string &key, const string &val);
        void keys(vector<string> &out) const;
    };
};

#endif // __DS_META_H__
