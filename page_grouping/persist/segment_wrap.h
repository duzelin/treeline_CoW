#pragma once

#include <cstdint>

#include "treeline/pg_db.h"
#include "mapping_table.h"
#include "page.h"

namespace tl {
namespace pg {

// A thin wrapper around a group of pages (a segment) to provide utility methods
// for the segment as a whole.
class SegmentWrap {
 public:
  SegmentWrap(void* data, const size_t pages_in_segment);

  void SetMappingTable(uint32_t mappings);

  uint32_t GetSequenceNumber() const;
  void SetSequenceNumber(uint32_t sequence);

  bool CheckChecksum() const;
  void ComputeAndSetChecksum();

  // Sets all overflow values to "invalid" (indicating no overflow).
  void ClearAllOverflows();

  // Returns true if there exists at least one page in the segment that has an
  // overflow.
  bool HasOverflow() const;

  // Returns the number of pages in this segment that have an overflow.
  size_t NumOverflows() const;

  template <class Callable>
  void ForEachPage(const Callable& callable) {
    for (size_t i = 0; i < pages_in_segment_; ++i) {
      callable(i, PageAtIndex(i));
    }
  }

  // Retrieve the encoded "lower"/"upper" boundaries in the segment.
  Key EncodedBaseKey() const;
  Key EncodedUpperKey() const;

  MappingTable& GetMappingTable() { return mapping_table_; }

 private:
  void RestoreMappingTable();
  Page PageAtIndex(size_t index) const; // Get the Page by indirection.
  Page PageAtIndex_phy(size_t index) const; // Directly get the Page without indirection.
  uint32_t ComputeChecksum() const;

  void* data_;
  size_t pages_in_segment_; // Logical page numbers
  MappingTable mapping_table_;
};

}  // namespace pg
}  // namespace tl
