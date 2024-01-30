#pragma once
#include <string>
#include <stdexcept>

namespace simple_server {

class LineBuf {
public:
    static constexpr char kEndMark = '\n';

    using Buf = std::string;

    explicit LineBuf(size_t len)
        : data_(len, 0)
    {}

    Buf & GetBuf()
    {
        return data_;
    }
    void Commit(size_t len)
    {
        if (len > data_.size()) {
            throw std::runtime_error("LineBuf::Commit len > size()");
        }
        rdSlice_ = std::string_view(data_).substr(0, len);
    }

    std::string_view Read()
    {
        auto pos = rdSlice_.find(kEndMark);
        if (pos == std::string_view::npos) {
            pos = rdSlice_.size();
        } else {
            pos += 1;
        }
        auto line = rdSlice_.substr(0, pos);
        rdSlice_ = rdSlice_.substr(pos);
        return line;
    }

    bool IsEmpty() const { return rdSlice_.empty(); }

private:
    std::string data_;
    std::string_view rdSlice_;
};

} //namespace simple_server
