#include <algorithm>
#include <iostream>

#include "helpers.h"
#include "wsum.h"
#include "walgebraicterm.h"
#include "wequationterm.h"
#include "wtensor.h"
#include "wsqoperator.h"

WSum::WSum() {}

std::vector<std::pair<WAlgebraicTerm, scalar_t>> WSum::vector() const {
  std::vector<std::pair<WAlgebraicTerm, scalar_t>> vec;
  for (auto &kv : sum_) {
    vec.push_back(kv);
  }
  return vec;
}

void WSum::add(const WAlgebraicTerm &term, scalar_t factor) {
  auto search = sum_.find(term);

  if (search != sum_.end()) {
    /// Found, then just add the factor to the existing term
    search->second += factor;
    if (search->second == 0) {
      std::cout << "\n Term cancellation!" << std::endl;
      sum_.erase(search);
    }
  } else {
    sum_[term] = factor;
  }
}

void WSum::add(const std::pair<WAlgebraicTerm, scalar_t> &term_factor,
               scalar_t scale) {

  const WAlgebraicTerm &term = term_factor.first;
  scalar_t factor = term_factor.second;

  auto search = sum_.find(term);

  if (search != sum_.end()) {
    /// Found, then just add the factor to the existing term
    search->second += scale * factor;
    if (search->second == 0) {
      std::cout << "\n Term cancellation!" << std::endl;
      sum_.erase(search);
    }
  } else {
    sum_[term] = scale * factor;
  }
}

void WSum::add_sum(WSum &&sum, scalar_t scale) {
  for (const auto &kv : sum.sum()) {
    add(kv, scale);
  }
}

bool WSum::operator==(WSum &sum) {
  return sum_.size() == sum.sum_.size() &&
         std::equal(sum_.begin(), sum_.end(), sum.sum_.begin());
}

WSum &WSum::operator+=(WSum &sum) {
  for (auto &kv : sum.sum()) {
    add(kv);
  }
  return *this;
}

WSum &WSum::operator+=(WSum &&sum) {
  for (auto &kv : sum.sum()) {
    add(kv);
  }
  return *this;
}

WSum &WSum::operator-=(WSum &sum) {
  for (auto &kv : sum.sum()) {
    add(kv, -1);
  }
  return *this;
}

WSum &WSum::operator-=(WSum &&sum) {
  for (auto &kv : sum.sum()) {
    add(kv, -1);
  }
  return *this;
}

std::string WSum::str() const {
  std::vector<std::string> str_vec;
  for (auto &kv : sum_) {
    str_vec.push_back(kv.second.str() + ' ' + kv.first.str());
  }
  return (to_string(str_vec, "\n"));
}

std::vector<WEquationTerm>
WSum::to_projective_equation(const std::string &residual_label) {
  std::vector<WEquationTerm> result;

  return result;
}

std::vector<WEquationTerm> WSum::to_manybody_equation() {
  std::vector<WEquationTerm> result;

  return result;
}

std::ostream &operator<<(std::ostream &os, const WSum &sum) {
  os << sum.str();
  return os;
}

WSum string_to_sum(const std::string &s, TensorSyntax syntax) {
  WSum sum;

  //"f^{v0}_{o0} t^{o0}_{v0}"
  std::string factor_re;
  std::string tensor_re;
  if (syntax == TensorSyntax::Wicked) {
    tensor_re = "([a-zA-Z0-9]+)\\^\\{([\\w,\\s]*)\\}_\\{([\\w,\\s]*)\\}";
    factor_re = "\\s*([+-]?\\d*)?/?(\\d*)?\\s+"; // ";
  }

  //  std::cout << "Parsing tensors: " << std::endl;
  auto tensors = findall(s, tensor_re);

  WAlgebraicTerm term;
  for (size_t n = 0; n < tensors.size(); n += 3) {
    std::string label = tensors[n];

    // Proecess the upper indices
    auto upper_idx = split_indices(tensors[n + 1]);
    std::vector<WIndex> upper;
    for (const auto &idx : upper_idx) {
      upper.push_back(string_to_index(idx));
    }

    // Process the lower indices
    auto lower_idx = split_indices(tensors[n + 2]);
    std::vector<WIndex> lower;
    for (const auto &idx : lower_idx) {
      lower.push_back(string_to_index(idx));
    }
    term.add(WTensor(label, lower, upper));
  }
  //  std::cout << "Parsing factor: " << std::endl;

  auto factor_vec = findall(s, factor_re);
  //  for (auto f : factor) {
  //    std::cout << "Factor: " << f << std::endl;
  //  }
  int numerator = 1;
  int denominator = 1;
  if (factor_vec[0] != "") {
    numerator = std::stoi(factor_vec[0]);
  }
  if (factor_vec[1] != "") {
    denominator = std::stoi(factor_vec[1]);
  }
  scalar_t factor(numerator, denominator);

  sum.add(term, factor);
  return sum;
}
