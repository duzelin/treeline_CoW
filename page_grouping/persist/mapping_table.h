#pragma once

#include <cstdint>
#include <cstddef>
#include <atomic>
#include <vector>

namespace tl {
namespace pg {

class MappingTable {
    public:
    MappingTable() {mappings_.store(0); version_num_.store(0); };
    MappingTable(uint32_t value, uint32_t num) { mappings_.store(value); version_num_.store(num); }
    MappingTable(MappingTable& mt) { mappings_.store(mt.GetValue()); version_num_.store(mt.GetVersion()); }
    MappingTable(MappingTable&& mt) { mappings_.store(mt.GetValue()); version_num_.store(mt.GetVersion()); }
    MappingTable& operator=(const MappingTable& mt) {
        if (this != &mt) {
            this->mappings_.store(mt.GetValue());
            this->version_num_.store(mt.GetVersion());
        }
        return *this;
    }
    MappingTable& operator=(const MappingTable&& mt) {
        if (this != &mt) {
            this->mappings_.store(mt.GetValue());
            this->version_num_.store(mt.GetVersion());
        }
        return *this;
    }
    void SetValue(uint32_t value) { mappings_.store(value); }
    void SetVersion(uint32_t version_num) { version_num_.store(version_num); }
    uint32_t GetValue() const { return mappings_.load(); }
    uint32_t GetVersion() const { return version_num_.load(); }
    uint32_t ReversePages(std::vector<uint32_t>& ModifiedPages) {
        bool exchanged = false;
        uint32_t new_mappings = 0;
        while (!exchanged) {
            uint32_t ori_mappings = mappings_.load(); 
            uint32_t reverse_mask = 0;
            for (auto& page : ModifiedPages) {
                reverse_mask |= (1 << page);
            }
            new_mappings = ori_mappings ^ reverse_mask;
            exchanged = mappings_.compare_exchange_strong(ori_mappings, new_mappings);
        }
        return new_mappings;
    }
    uint32_t ReversePage(uint32_t ModifiedPage) {
        bool exchanged = false;
        uint32_t new_mappings = 0;
        while (!exchanged) {
            uint32_t ori_mappings = mappings_.load();
            uint32_t reverse_mask = 0; 
            reverse_mask = (1 << ModifiedPage);
            new_mappings = ori_mappings ^ reverse_mask;
            exchanged = mappings_.compare_exchange_strong(ori_mappings, new_mappings);
        }
        return new_mappings;
    }
    uint32_t ReversePageTmp(uint32_t ModifiedPage) {
        uint32_t mappings_tmp_ = mappings_.load();
        uint32_t reverse_mask = (1 << ModifiedPage);
        mappings_tmp_ = mappings_tmp_ ^ reverse_mask;
        return mappings_tmp_;
    }
    // Translate loggical page number to physical page number.
    // SegmentLength : logical page numbers of the segment (i.e. 1, 2, 4, 8, 16)
    // If the corresponding bit is 0, then physical page = logical page;
    // Else, physical page = logical page + segment length.
    uint32_t GetPhyPageNum(uint32_t LogPageNum, size_t SegmentLength) const {
        uint32_t mappings = mappings_.load();
        return (mappings & (1 << LogPageNum)) ? (LogPageNum + SegmentLength) : LogPageNum;
    }

    uint32_t GetBackupPageNum(uint32_t LogPageNum, size_t SegmentLength) const {
        uint32_t mappings = mappings_.load();
        return (mappings & (1 << LogPageNum)) ? LogPageNum: (LogPageNum + SegmentLength);
    }

    uint32_t GetOverflowPageNum(uint32_t LogPageNum) const{
        uint32_t mappings = mappings_.load();
        return (mappings & (1 << LogPageNum + 16) ? 1 : 0);
    }

    uint32_t BindingNewVersion() {
        return (++version_num_);
    }

    private:
    std::atomic<uint32_t> mappings_;
    std::atomic<uint32_t> version_num_;    
};

}
} // namespace tl