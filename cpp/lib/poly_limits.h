// Constants in this file could be adjusted to make specific computations run
// faster and use less RAM. The defaults are such that a wide range of
// computations is allowed, but it's not maximally efficient.
//
// Note that tests are not guaranteed to work with all combinations of these
// parameters.
//
// An expression of weight N is usually a vector of length N. A coexpression
// with M parts is a vector of M primitive expressions of weight N.
//
// Polykit uses three kind of vectors:
//   * `std::vector`: Regular vector. It uses 24 bytes on a 64-bit system plus
//     one heap allocation.
//   * `absl::InlinedVector`: A vector that utilizes inline storage from small
//     sizes. Is uses at least 24 bytes on a 64-bit system, but it can use more
//     if large inline capacity is requested. 8 bytes are used for the size, the
//     rest can be used to store inline data. We make a tweak on top of the
//     library version of the inlined vector: inlined capacity is automatically
//     bumped to avoid wasting space. E.g. if a 3-element vector is requested
//     and each element size is 2 bytes, we would automatically allocate inline
//     storage for 8 objects because it's free (vector size is 24 bytes in
//     either case).
//   * `CappedVector`: An always-inline vector that panics when capacity is
//     exceeded. The size is stored as a single byte, so this vector is much
//     more memory efficient for small sizes. E.g. `CappedVector<chr, 3>` is
//     just 4 bytes (c.f. 24 bytes for `absl::InlinedVector`).
//
// Vectors are chosen based on kMaxWeight/kDynamicWeight (similarly for
// kMaxCoparts/kDynamicCoparts) according to the following logic :
//   * If kDynamicWeight is true, use `PVector`, which in turn uses:
//     - `absl::InlinedVector` if kMaxWeight is a small positive number.
//     - `std::vector` is kMaxWeight is 0 or large (namely, if
//       `absl::InlinedVector` object size were to be above `kMaxPVectorSize`)
//   * If kDynamicWeight is false, use `CappedVector`.

#pragma once


static constexpr int kMaxVariables = 16;  // ignored by DeltaExpr for now
static constexpr bool kZeroBasedVariables = true;
static constexpr bool kAllowNegativeVariables = true;

static constexpr int kMaxWeight = 8;
static constexpr bool kDynamicWeight = true;

static constexpr int kMaxCoparts = 2;
static constexpr bool kDynamicCoparts = true;


static_assert(kMaxWeight > 0 || kDynamicWeight);
static_assert(kMaxCoparts >= 2 || kDynamicCoparts);
