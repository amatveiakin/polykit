#pragma once

#include "absl/container/flat_hash_map.h"

#include "string.h"


class TablePrinter {
public:
  struct CellCoord {
    int row = 0;
    int col = 0;

    bool operator==(const CellCoord& other) const = default;

    template <typename H>
    friend H AbslHashValue(H h, CellCoord cell) {
      return H::combine(std::move(h), cell.row, cell.col);
    }
  };

  std::string content(CellCoord cell) const;
  void set_content(CellCoord cell, std::string content);

  // Disable alignment in order to export into spreadsheet systems like Excel.
  // Recommended: also set set_column_separator to "\t".
  void set_enable_alignment(bool v);

  // Alignment priority:
  //   cell, row, column, table default, built-in default (left).
  void set_default_alignment(TextAlignment alignment);
  void set_column_alignment(int col, TextAlignment alignment);
  void set_row_alignment(int row, TextAlignment alignment);
  void set_cell_alignment(CellCoord cell, TextAlignment alignment);

  void set_min_column_width(int width);
  void set_column_separator(std::string separator);

  std::string to_string() const;

private:
  TextAlignment alignment_for_cell(CellCoord cell) const;

  bool enable_alignment_ = true;
  TextAlignment default_alignment_ = TextAlignment::left;
  absl::flat_hash_map<int, TextAlignment> column_alignment_;
  absl::flat_hash_map<int, TextAlignment> row_alignment_;
  absl::flat_hash_map<CellCoord, TextAlignment> cell_alignment_;

  int min_column_width_ = 0;
  std::string column_separator_ = " ";
  absl::flat_hash_map<CellCoord, std::string> content_table_;
};
