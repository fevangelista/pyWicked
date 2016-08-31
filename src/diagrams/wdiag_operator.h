#ifndef _wicked_diag_operator_h_
#define _wicked_diag_operator_h_

#include <vector>
#include <string>

#include "wicked-def.h"

/// A class to represent operators
class WDiagOperator {

public:
  /// Constructor. Set number of
  WDiagOperator(const std::string &label, const std::vector<int> &cre,
                const std::vector<int> &ann);

  /// Return the label of the operator
  std::string &label();

  /// Return the number of indices for a given space and type (cre/ann)
  int num_indices(int space, bool creation) const;

  /// Return a string representation of the operator
  std::string str() const;

  /// Write a string representation of the operator to a stream
  friend std::ostream &operator<<(std::ostream &os, const WDiagOperator &op);

private:
  /// The label of the operator
  std::string label_;

  /// The number of creation/annihilation operators in each space
  std::vector<std::pair<int, int>> ops_;
};

// Helper functions
/// Create an operator
WDiagOperator make_diag_operator(const std::string &label,
                                 const std::vector<std::string> &cre_labels,
                                 const std::vector<std::string> &ann_labels);

#endif // _wicked_diag_operator_h_