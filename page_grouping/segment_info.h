#pragma once

#include <optional>

#include "key.h"
#include "persist/segment_id.h"
#include "persist/mapping_table.h"
#include "plr/data.h"

namespace tl {
namespace pg {

class SegmentInfo {
 public:
  SegmentInfo(SegmentId id, std::optional<plr::Line64> model)
      : raw_id_(id.value() & kSegmentIdMask), model_(model) {}

  // Represents an invalid `SegmentInfo`.
  SegmentInfo() : raw_id_(kInvalidSegmentId) {}

  SegmentId id() const {
    return raw_id_ == kInvalidSegmentId ? SegmentId()
                                        : SegmentId(raw_id_ & kSegmentIdMask);
  }

  size_t page_count() const { return 1ULL << id().GetFileId(); }

  const std::optional<plr::Line64>& model() const { return model_; }

  // Get the logical page number via linear model.
  size_t PageForKey(Key base_key, Key candidate) const {
    const size_t pages = page_count();
    size_t logical_page = 0;
    if (pages == 1) {
      logical_page = 0;
    } else {
      logical_page = pg::PageForKey(base_key, *model_, pages, candidate);
    }
    return logical_page;
  }

  // Translate the logical page number to the physical page number via mapping table.
  size_t GetPhyPage(size_t logical_page) const {
    const size_t pages = page_count();
    return mapping_table_.GetPhyPageNum(logical_page, pages);
  }

  size_t GetBackupPage(size_t logical_page) const {
    const size_t pages = page_count();
    return mapping_table_.GetBackupPageNum(logical_page, pages);
  }

  void UpdateMappingTable(std::vector<uint32_t>& ModifiedPages) {
    mapping_table_.ReversePages(ModifiedPages);
  }

  void UpadteBitMappingTable(uint32_t ModifiedPage) {
    mapping_table_.ReversePage(ModifiedPage);
  }

  uint32_t UpadteBitMappingTableTmp(uint32_t ModifiedPage){
    return mapping_table_.ReversePageTmp(ModifiedPage);
  }

  void BindNewVersion() {
    mapping_table_.BindingNewVersion();
  }

  uint32_t GetMappingValue() {
    return mapping_table_.GetValue();
  }

  uint32_t GetVersionNum() {
    return mapping_table_.GetVersion();
  }

  void SetMappingValue(uint32_t mappings) {
    mapping_table_.SetValue(mappings);
  }

  void SetVersionNum(uint32_t versions) {
     mapping_table_.SetVersion(versions);
  }

  bool operator==(const SegmentInfo& other) const {
    return raw_id_ == other.raw_id_ && model_ == other.model_;
  }

  void SetOverflow(bool overflow) {
    if (overflow) {
      raw_id_ |= kHasOverflowMask;
    } else {
      raw_id_ &= kSegmentIdMask;
    }
  }

  bool HasOverflow() const { return (raw_id_ & kHasOverflowMask) != 0; }

  MappingTable& GetMappingTable() { return mapping_table_; }

 private:
  // Most significant bit used to indicate whether or not this segment has an
  // overflow. The remaining 63 bits hold the `SegmentId` value. If all 63 bits
  // are set to 1, we assume the ID is invalid.
  size_t raw_id_;
  std::optional<plr::Line64> model_;
  MappingTable mapping_table_;

  static constexpr size_t kHasOverflowMask = (1ULL << 63);
  static constexpr size_t kSegmentIdMask = ~(kHasOverflowMask);
  static constexpr size_t kInvalidSegmentId = kSegmentIdMask;
};

}  // namespace pg
}  // namespace tl
