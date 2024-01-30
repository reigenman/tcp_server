#pragma once
#include <boost/core/span.hpp>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace simple_server {


class LineBuf {
public:
    static constexpr uint8_t kEndMark = '\n';

    using ByteBuf = std::vector<uint8_t>;
    using Span = boost::span<const uint8_t>;

    explicit LineBuf(size_t len)
        : data_(len, 0)
    {}

    ByteBuf & GetBuf()
    {
        return data_;
    }
    void Commit(size_t len)
    {
        if (len > data_.size()) {
            throw std::runtime_error("LineBuf::Commit len > size()");
        }
        rdSlice_ = Span(data_).subspan(0, len);
    }

    Span Read()
    {
        auto it = std::find(rdSlice_.begin(), rdSlice_.end(), kEndMark);
        if (it == rdSlice_.end()) {
            auto line = rdSlice_;
            rdSlice_ = Span();
            return line;
        }
        size_t lineSize = std::distance(std::begin(rdSlice_), it) + 1;
        auto line = rdSlice_.first(lineSize);
        rdSlice_ = rdSlice_.last(rdSlice_.size() - lineSize);
        return line;
    }

    bool IsEmpty() const { return rdSlice_.empty(); }

private:
    std::vector<uint8_t> data_;
    Span rdSlice_;
};

} //namespace simple_server
