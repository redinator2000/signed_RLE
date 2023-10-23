#include "signed_RLE.hpp"
#include <limits>
#include <string.h>
#include <stdlib.h>

namespace signed_RLE
{
template<class T>
size_t push_end_chars(std::vector<int8_t> * v, const T data)
{
    size_t start = v->size();
    v->insert(v->end(), (int8_t*)&data, (int8_t*)&data + sizeof(T));
    return start;
}
std::vector<int8_t> uncompress(const std::vector<int8_t> & compressed, int limit)
{
    std::vector<int8_t> out = {};

    enum class Uncomp_State
    {
        start,
        big_number,
        repeat_data,
        unique_data
    };
    Uncomp_State state = Uncomp_State::start;
    int count;
    int8_t bigcount[sizeof(int32_t)];
    int bigcount_builder;

    for(auto c : compressed)
    {
        switch(state)
        {
        default:
        case Uncomp_State::start:
            if(c == 0)
            {
                state = Uncomp_State::big_number;
                bigcount_builder = 0;
            }
            else if(c > 0)
            {
                count = c;
                state = Uncomp_State::repeat_data;
            }
            else//(c < 0)
            {
                count = abs(c);
                state = Uncomp_State::unique_data;
            }
            break;
        case Uncomp_State::big_number:
            //printf("building big number '%d'\n", c);
            bigcount[bigcount_builder] = c;
            bigcount_builder++;
            if(bigcount_builder >= sizeof(int32_t))
            {
                int32_t copycount;
                memcpy(&copycount, bigcount, sizeof(int32_t));
                //printf("built big number %d\n", copycount);
                if(copycount == 0)
                {
                    return out;
                }
                else if(copycount > 0)
                {
                    count = copycount;
                    state = Uncomp_State::repeat_data;
                }
                else//(copycount < 0)
                {
                    count = abs(copycount);
                    state = Uncomp_State::unique_data;
                }
            break;
            }
            break;
        case Uncomp_State::repeat_data:
            //printf("putting in repeat %d x '%d'\n", count, c);
            out.insert(out.end(), count, c);
            if(limit && out.size() > limit)
                return out;
            state = Uncomp_State::start;
            break;
        case Uncomp_State::unique_data:
            //printf("putting in unique '%d', %d remaining\n", c, count);
            out.push_back(c);
            if(limit && out.size() > limit)
                return out;
            count--;
            if(count <= 0)
                state = Uncomp_State::start;
            break;
        }
    }
    return out;
}
std::vector<int8_t> compress(const std::vector<int8_t> & raw)
{
    std::vector<int8_t> out = {};
    if(raw.empty())
        return out;

    struct Segment
    {
        int32_t count = 0;
        int8_t similar;
        std::vector<int8_t> uniques = {};
    };
    std::vector<Segment> segments = {};
    {
        Segment builder_segments;
        builder_segments.similar = raw[0] + 1;
        for(auto r : raw)
        {
            if(r == builder_segments.similar)
                builder_segments.count++;//todo: split segments if goes above int32_t count
            else
            {
                if(builder_segments.count)
                    segments.push_back(builder_segments);
                builder_segments.count = 1;
                builder_segments.similar = r;
                builder_segments.uniques.clear();
            }
        }
        if(builder_segments.count)
            segments.push_back(builder_segments);
    }

    std::vector<Segment> uniqued_segments = {};
    {
        Segment builder_uniqued;
        builder_uniqued.count = 0;
        for(auto s : segments)
        {
            if(s.count > 3)
            {
                if(builder_uniqued.count)
                {
                    uniqued_segments.push_back(builder_uniqued);
                    builder_uniqued.count = 0;
                    builder_uniqued.uniques.clear();
                }
                uniqued_segments.push_back(s);
            }
            else
            {
                builder_uniqued.count -= s.count;
                builder_uniqued.uniques.insert(builder_uniqued.uniques.end(), s.count, s.similar);
            }
        }
        if(builder_uniqued.count)
            uniqued_segments.push_back(builder_uniqued);
    }

    for(auto s : uniqued_segments)
    {
        if(s.count > 0)
        {
            if(s.count >= std::numeric_limits<int8_t>::max())
            {
                out.push_back(0);
                push_end_chars<int32_t>(&out, s.count);
            }
            else
                out.push_back(static_cast<int8_t>(s.count));
            out.push_back(s.similar);
        }
        else
        {
            if(s.count <= std::numeric_limits<int8_t>::min())
            {
                out.push_back(0);
                push_end_chars<int32_t>(&out, s.count);
            }
            else
                out.push_back(static_cast<int8_t>(s.count));
            out.insert(out.end(), s.uniques.begin(), s.uniques.end());
        }
    }
    return out;
}
};
