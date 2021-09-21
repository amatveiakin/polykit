#include "table_printer.h"

#include "util.h"

using CellCoord = TablePrinter::CellCoord;


std::string TablePrinter::content(CellCoord cell) const { return value_or(content_table_, cell); }
void TablePrinter::set_content(CellCoord cell, std::string content) { content_table_[cell] = std::move(content); }

void TablePrinter::set_enable_alignment(bool v) { enable_alignment_ = v; }

void TablePrinter::set_default_alignment(TextAlignment alignment) { default_alignment_ = alignment; }
void TablePrinter::set_column_alignment(int col, TextAlignment alignment) { column_alignment_[col] = alignment; }
void TablePrinter::set_row_alignment(int row, TextAlignment alignment) { row_alignment_[row] = alignment; }
void TablePrinter::set_cell_alignment(CellCoord cell, TextAlignment alignment) { cell_alignment_[cell] = alignment; }

void TablePrinter::set_min_column_width(int width) { min_column_width_ = width; }
void TablePrinter::set_column_separator(std::string separator) { column_separator_ = std::move(separator); }

std::string TablePrinter::to_string() const {
  int num_rows = 0;
  int num_columns = 0;
  absl::flat_hash_map<int, int> column_width;
  for (const auto& [cell, cell_content] : content_table_) {
    const auto [row, col] = cell;
    num_rows = std::max(num_rows, row + 1);
    num_columns = std::max(num_columns, col + 1);
    if (enable_alignment_) {
      auto& width = column_width[col];
      width = std::max(width, strlen_utf8(cell_content));
    }
  }
  std::string table_out;
  for (const int row : range(num_rows)) {
    std::string row_out;
    for (const int col : range(num_columns)) {
      if (!row_out.empty()) {
        row_out += column_separator_;
      }
      const CellCoord cell{row, col};
      row_out += pad_string(
        alignment_for_cell(cell),
        content(cell),
        std::max(min_column_width_, value_or(column_width, col))
      );
    }
    table_out += row_out + "\n";
  }
  return table_out;
}

TextAlignment TablePrinter::alignment_for_cell(CellCoord cell) const {
  if (const auto it = cell_alignment_.find(cell); it != cell_alignment_.end()) {
    return it->second;
  }
  if (const auto it = row_alignment_.find(cell.row); it != row_alignment_.end()) {
    return it->second;
  }
  if (const auto it = column_alignment_.find(cell.col); it != column_alignment_.end()) {
    return it->second;
  }
  return default_alignment_;
}
