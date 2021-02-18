//==---- algorithm.h ------------------------------*- C++ -*----------------==//
//
// Copyright (C) 2019 - 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// See https://llvm.org/LICENSE.txt for license information.
//
//===----------------------------------------------------------------------===//

#ifndef __DPCT_ALGORITHM_H__
#define __DPCT_ALGORITHM_H__

#include <oneapi/dpl/execution>
#include <oneapi/dpl/algorithm>
#include <oneapi/dpl/numeric>

#include "functional.h"
#include "iterators.h"

namespace dpct {

template <typename Policy, typename Iter1, typename Iter2, typename Pred,
          typename T>
void replace_if(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask, Pred p,
                const T &new_value) {
  std::transform(std::forward<Policy>(policy), first, last, mask, first,
                 internal::replace_if_fun<T, Pred>(p, new_value));
}

template <typename Policy, typename Iter1, typename Iter2, typename Iter3,
          typename Pred, typename T>
Iter3 replace_copy_if(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask,
                      Iter3 result, Pred p, const T &new_value) {
  return std::transform(std::forward<Policy>(policy), first, last, mask, result,
                        internal::replace_if_fun<T, Pred>(p, new_value));
}

template <typename Policy, typename Iter1, typename Iter2, typename Pred>
Iter1 remove_if(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask, Pred p) {
  using oneapi::dpl::make_zip_iterator;
  using policy_type = typename std::decay<Policy>::type;
  using internal::__buffer;
  using ValueType = typename std::iterator_traits<Iter1>::value_type;

  __buffer<ValueType> _tmp(std::distance(first, last));

  typename internal::rebind_policy<policy_type, class RemoveIf1>::type policy1(
      policy);
  auto end = std::copy_if(
      policy1, make_zip_iterator(first, mask),
      make_zip_iterator(last, mask + std::distance(first, last)),
      make_zip_iterator(_tmp.get(), oneapi::dpl::discard_iterator()),
      internal::negate_predicate_key_fun<Pred>(p));
  typename internal::rebind_policy<policy_type, class RemoveIf2>::type policy2(
      policy);
  return std::copy(policy2, _tmp.get(), std::get<0>(end.base()), first);
}

template <typename Policy, typename Iter1, typename Iter2, typename Iter3,
          typename Pred>
Iter3 remove_copy_if(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask,
                     Iter3 result, Pred p) {
  using oneapi::dpl::make_zip_iterator;
  auto ret_val = std::remove_copy_if(
      std::forward<Policy>(policy), make_zip_iterator(first, mask),
      make_zip_iterator(last, mask + std::distance(first, last)),
      make_zip_iterator(result, oneapi::dpl::discard_iterator()),
      internal::predicate_key_fun<Pred>(p));
  return std::get<0>(ret_val.base());
}

template <class Policy, class Iter1, class Iter2, class BinaryPred>
std::pair<Iter1, Iter2> unique(Policy &&policy, Iter1 keys_first,
                               Iter1 keys_last, Iter2 values_first,
                               BinaryPred binary_pred) {
  auto ret_val = std::unique(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first, values_first),
      oneapi::dpl::make_zip_iterator(
          keys_last, values_first + std::distance(keys_first, keys_last)),
      internal::compare_key_fun<BinaryPred>(binary_pred));
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_first, values_first), ret_val);
  return std::make_pair(keys_first + n1, values_first + n1);
}

template <class Policy, class Iter1, class Iter2>
std::pair<Iter1, Iter2> unique(Policy &&policy, Iter1 keys_first,
                               Iter1 keys_last, Iter2 values_first) {
  using T = typename std::iterator_traits<Iter1>::value_type;
  return unique(std::forward<Policy>(policy), keys_first, keys_last,
                values_first, std::equal_to<T>());
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class BinaryPred>
std::pair<Iter1, Iter2> unique_copy(Policy &&policy, Iter1 keys_first,
                                    Iter1 keys_last, Iter2 values_first,
                                    Iter3 keys_result, Iter4 values_result,
                                    BinaryPred binary_pred) {
  auto ret_val = std::unique_copy(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first, values_first),
      oneapi::dpl::make_zip_iterator(
          keys_last, values_first + std::distance(keys_first, keys_last)),
      oneapi::dpl::make_zip_iterator(keys_result, values_result),
      internal::unique_fun<BinaryPred>(binary_pred));
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_result, values_result), ret_val);
  return std::make_pair(keys_result + n1, values_result + n1);
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4>
std::pair<Iter1, Iter2> unique_copy(Policy &&policy, Iter1 keys_first,
                                    Iter1 keys_last, Iter2 values_first,
                                    Iter3 keys_result, Iter4 values_result) {
  using T = typename std::iterator_traits<Iter1>::value_type;
  auto comp = std::equal_to<T>();
  return unique_copy(std::forward<Policy>(policy), keys_first, keys_last,
                     values_first, keys_result, values_result, comp);
}

template <typename Policy, typename Iter, typename Pred>
Iter partition_point(Policy &&policy, Iter first, Iter last, Pred p) {
  if (std::is_partitioned(std::forward<Policy>(policy), first, last, p))
    return std::find_if_not(std::forward<Policy>(policy), first, last, p);
  else
    return first;
}

template <typename Policy, typename Iter1, typename Iter2, typename Iter3,
          typename Pred>
Iter3 copy_if(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask,
              Iter3 result, Pred pred) {
  auto ret_val = std::copy_if(
      std::forward<Policy>(policy), oneapi::dpl::make_zip_iterator(first, mask),
      oneapi::dpl::make_zip_iterator(last, mask + std::distance(first, last)),
      oneapi::dpl::make_zip_iterator(result, oneapi::dpl::discard_iterator()),
      internal::predicate_key_fun<Pred>(pred));
  return std::get<0>(ret_val.base());
}

template <class Policy, class Iter1, class Iter2, class UnaryOperation>
Iter2 transform(Policy &&policy, Iter1 first, Iter1 last, Iter2 result,
                UnaryOperation unary_op) {
  return std::transform(std::forward<Policy>(policy), first, last, result,
                        unary_op);
}

template <class Policy, class Iter1, class Iter2, class Iter3,
          class UnaryOperation>
Iter3 transform(Policy &&policy, Iter1 first1, Iter1 last1, Iter2 first2,
                Iter3 result, UnaryOperation unary_op) {
  return std::transform(std::forward<Policy>(policy), first1, last1, first2,
                        result, unary_op);
}

template <class Policy, class Iter1, class Iter2, class UnaryOperation,
          class Pred>
Iter2 transform_if(Policy &&policy, Iter1 first, Iter1 last, Iter2 result,
                   UnaryOperation unary_op, Pred pred) {
  using T = typename std::iterator_traits<Iter1>::value_type;
  return std::transform(
      std::forward<Policy>(policy), first, last, result,
      internal::transform_if_fun<T, Pred, UnaryOperation>(pred, unary_op));
}

template <class Policy, class Iter1, class Iter2, class Iter3,
          class UnaryOperation, class Pred>
Iter3 transform_if(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask,
                   Iter3 result, UnaryOperation unary_op, Pred pred) {
  using T = typename std::iterator_traits<Iter1>::value_type;
  using Ref1 = typename std::iterator_traits<Iter1>::reference;
  using Ref2 = typename std::iterator_traits<Iter2>::reference;
  return std::transform(
      std::forward<Policy>(policy), first, last, mask, result,
      internal::transform_if_mask_fun<T, Pred, UnaryOperation>(pred, unary_op));
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class BinaryOperation, class Pred>
Iter4 transform_if(Policy &&policy, Iter1 first1, Iter1 last1, Iter2 first2,
                   Iter3 mask, Iter4 result, BinaryOperation binary_op,
                   Pred pred) {
  const auto n = std::distance(first1, last1);
  using ZipIterator =
      typename oneapi::dpl::zip_iterator<Iter1, Iter2, Iter3, Iter4>;
  using T = typename std::iterator_traits<ZipIterator>::value_type;
  std::for_each(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(first1, first2, mask, result),
      oneapi::dpl::make_zip_iterator(last1, first2 + n, mask + n, result + n),
      internal::transform_if_zip_mask_fun<T, Pred, BinaryOperation>(pred,
                                                                    binary_op));
  return result + n;
}

template <typename Policy, typename Iter1, typename Iter2, typename Iter3,
          typename Iter4, typename Iter5, typename Iter6>
std::pair<Iter5, Iter6>
merge(Policy &&policy, Iter1 keys_first1, Iter1 keys_last1, Iter2 keys_first2,
      Iter2 keys_last2, Iter3 values_first1, Iter4 values_first2,
      Iter5 keys_result, Iter6 values_result) {
  auto n1 = std::distance(keys_first1, keys_last1);
  auto n2 = std::distance(keys_first2, keys_last2);
  std::merge(std::forward<Policy>(policy),
             oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
             oneapi::dpl::make_zip_iterator(keys_last1, values_first1 + n1),
             oneapi::dpl::make_zip_iterator(keys_first2, values_first2),
             oneapi::dpl::make_zip_iterator(keys_last2, values_first2 + n2),
             oneapi::dpl::make_zip_iterator(keys_result, values_result),
             internal::compare_key_fun<>());
  return std::make_pair(keys_result + n1 + n2, values_result + n1 + n2);
}

template <typename Policy, typename Iter1, typename Iter2, typename Iter3,
          typename Iter4, typename Iter5, typename Iter6, typename Comp>
std::pair<Iter5, Iter6>
merge(Policy &&policy, Iter1 keys_first1, Iter1 keys_last1, Iter2 keys_first2,
      Iter2 keys_last2, Iter3 values_first1, Iter4 values_first2,
      Iter5 keys_result, Iter6 values_result, Comp comp) {
  auto n1 = std::distance(keys_first1, keys_last1);
  auto n2 = std::distance(keys_first2, keys_last2);
  std::merge(std::forward<Policy>(policy),
             oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
             oneapi::dpl::make_zip_iterator(keys_last1, values_first1 + n1),
             oneapi::dpl::make_zip_iterator(keys_first2, values_first2),
             oneapi::dpl::make_zip_iterator(keys_last2, values_first2 + n2),
             oneapi::dpl::make_zip_iterator(keys_result, values_result),
             internal::compare_key_fun<Comp>(comp));
  return std::make_pair(keys_result + n1 + n2, values_result + n1 + n2);
}

template <class Policy, class Iter, class T>
void iota(Policy &&policy, Iter first, Iter last, T init, T step) {
  using DiffSize = typename std::iterator_traits<Iter>::difference_type;
  std::transform(
      std::forward<Policy>(policy), oneapi::dpl::counting_iterator<DiffSize>(0),
      oneapi::dpl::counting_iterator<DiffSize>(std::distance(first, last)),
      first, internal::sequence_fun<T>(init, step));
}

template <class Policy, class Iter, class T>
void iota(Policy &&policy, Iter first, Iter last, T init) {
  iota(std::forward<Policy>(policy), first, last, init, T(1));
}

template <class Policy, class Iter>
void iota(Policy &&policy, Iter first, Iter last) {
  using DiffSize = typename std::iterator_traits<Iter>::difference_type;
  iota(std::forward<Policy>(policy), first, last, DiffSize(0), DiffSize(1));
}

template <class Policy, class Iter1, class Iter2, class Comp>
void sort(Policy &&policy, Iter1 keys_first, Iter1 keys_last,
          Iter2 values_first, Comp comp) {
  auto first = oneapi::dpl::make_zip_iterator(keys_first, values_first);
  auto last = first + std::distance(keys_first, keys_last);
  std::sort(std::forward<Policy>(policy), first, last,
            internal::compare_key_fun<Comp>(comp));
}

template <class Policy, class Iter1, class Iter2>
void sort(Policy &&policy, Iter1 keys_first, Iter1 keys_last,
          Iter2 values_first) {
  sort(std::forward<Policy>(policy), keys_first, keys_last, values_first,
       internal::__less());
}

template <class Policy, class Iter1, class Iter2, class Comp>
void stable_sort(Policy &&policy, Iter1 keys_first, Iter1 keys_last,
                 Iter2 values_first, Comp comp) {
  std::stable_sort(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first, values_first),
      oneapi::dpl::make_zip_iterator(
          keys_last, values_first + std::distance(keys_first, keys_last)),
      internal::compare_key_fun<Comp>(comp));
}

template <class Policy, class Iter1, class Iter2>
void stable_sort(Policy &&policy, Iter1 keys_first, Iter1 keys_last,
                 Iter2 values_first) {
  stable_sort(std::forward<Policy>(policy), keys_first, keys_last, values_first,
              internal::__less());
}

template <class Policy, class Iter, class Operator>
void for_each_index(Policy &&policy, Iter first, Iter last, Operator unary_op) {
  using DiffSize = typename std::iterator_traits<Iter>::difference_type;
  std::transform(
      std::forward<Policy>(policy), oneapi::dpl::counting_iterator<DiffSize>(0),
      oneapi::dpl::counting_iterator<DiffSize>(std::distance(first, last)),
      first, unary_op);
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class Iter5>
std::pair<Iter4, Iter5>
set_intersection(Policy &&policy, Iter1 keys_first1, Iter1 keys_last1,
                 Iter2 keys_first2, Iter2 keys_last2, Iter3 values_first1,
                 Iter4 keys_result, Iter5 values_result) {
  auto ret_val = std::set_intersection(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
      oneapi::dpl::make_zip_iterator(
          keys_last1, values_first1 + std::distance(keys_first1, keys_last1)),
      oneapi::dpl::make_zip_iterator(keys_first2,
                                     oneapi::dpl::discard_iterator()),
      oneapi::dpl::make_zip_iterator(keys_last2,
                                     oneapi::dpl::discard_iterator()),
      oneapi::dpl::make_zip_iterator(keys_result, values_result),
      internal::compare_key_fun<>());
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_result, values_result), ret_val);
  return std::make_pair(keys_result + n1, values_result + n1);
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class Iter5, class Comp>
std::pair<Iter4, Iter5>
set_intersection(Policy &&policy, Iter1 keys_first1, Iter1 keys_last1,
                 Iter2 keys_first2, Iter2 keys_last2, Iter3 values_first1,
                 Iter4 keys_result, Iter5 values_result, Comp comp) {
  auto ret_val = std::set_intersection(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
      oneapi::dpl::make_zip_iterator(
          keys_last1, values_first1 + std::distance(keys_first1, keys_last1)),
      oneapi::dpl::make_zip_iterator(keys_first2,
                                     oneapi::dpl::discard_iterator()),
      oneapi::dpl::make_zip_iterator(keys_last2,
                                     oneapi::dpl::discard_iterator()),
      oneapi::dpl::make_zip_iterator(keys_result, values_result),
      internal::compare_key_fun<Comp>(comp));
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_result, values_result), ret_val);
  return std::make_pair(keys_result + n1, values_result + n1);
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class Iter5, class Iter6>
std::pair<Iter5, Iter6>
set_symmetric_difference(Policy &&policy, Iter1 keys_first1, Iter1 keys_last1,
                         Iter2 keys_first2, Iter2 keys_last2,
                         Iter3 values_first1, Iter4 values_first2,
                         Iter5 keys_result, Iter6 values_result) {
  auto ret_val = std::set_symmetric_difference(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
      oneapi::dpl::make_zip_iterator(
          keys_last1, values_first1 + std::distance(keys_first1, keys_last1)),
      oneapi::dpl::make_zip_iterator(keys_first2, values_first2),
      oneapi::dpl::make_zip_iterator(
          keys_last2, values_first2 + std::distance(keys_first2, keys_last2)),
      oneapi::dpl::make_zip_iterator(keys_result, values_result),
      internal::compare_key_fun<>());
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_result, values_result), ret_val);
  return std::make_pair(keys_result + n1, values_result + n1);
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class Iter5, class Iter6, class Comp>
std::pair<Iter5, Iter6>
set_symmetric_difference(Policy &&policy, Iter1 keys_first1, Iter1 keys_last1,
                         Iter2 keys_first2, Iter2 keys_last2,
                         Iter3 values_first1, Iter4 values_first2,
                         Iter5 keys_result, Iter6 values_result, Comp comp) {
  auto ret_val = std::set_symmetric_difference(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
      oneapi::dpl::make_zip_iterator(
          keys_last1, values_first1 + std::distance(keys_first1, keys_last1)),
      oneapi::dpl::make_zip_iterator(keys_first2, values_first2),
      oneapi::dpl::make_zip_iterator(
          keys_last2, values_first2 + std::distance(keys_first2, keys_last2)),
      oneapi::dpl::make_zip_iterator(keys_result, values_result),
      internal::compare_key_fun<Comp>(comp));
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_result, values_result), ret_val);
  return std::make_pair(keys_result + n1, values_result + n1);
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class Iter5, class Iter6>
std::pair<Iter5, Iter6>
set_difference(Policy &&policy, Iter1 keys_first1, Iter1 keys_last1,
               Iter2 keys_first2, Iter2 keys_last2, Iter3 values_first1,
               Iter4 values_first2, Iter5 keys_result, Iter6 values_result) {
  auto ret_val = std::set_difference(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
      oneapi::dpl::make_zip_iterator(
          keys_last1, values_first1 + std::distance(keys_first1, keys_last1)),
      oneapi::dpl::make_zip_iterator(keys_first2, values_first2),
      oneapi::dpl::make_zip_iterator(
          keys_last2, values_first2 + std::distance(keys_first2, keys_last2)),
      oneapi::dpl::make_zip_iterator(keys_result, values_result),
      internal::compare_key_fun<>());
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_result, values_result), ret_val);
  return std::make_pair(keys_result + n1, values_result + n1);
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class Iter5, class Iter6, class Comp>
std::pair<Iter5, Iter6> set_difference(Policy &&policy, Iter1 keys_first1,
                                       Iter1 keys_last1, Iter2 keys_first2,
                                       Iter2 keys_last2, Iter3 values_first1,
                                       Iter4 values_first2, Iter5 keys_result,
                                       Iter6 values_result, Comp comp) {
  auto ret_val = std::set_difference(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
      oneapi::dpl::make_zip_iterator(
          keys_last1, values_first1 + std::distance(keys_first1, keys_last1)),
      oneapi::dpl::make_zip_iterator(keys_first2, values_first2),
      oneapi::dpl::make_zip_iterator(
          keys_last2, values_first2 + std::distance(keys_first2, keys_last2)),
      oneapi::dpl::make_zip_iterator(keys_result, values_result),
      internal::compare_key_fun<Comp>(comp));
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_result, values_result), ret_val);
  return std::make_pair(keys_result + n1, values_result + n1);
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class Iter5, class Iter6>
internal::enable_if_execution_policy<Policy, std::pair<Iter5, Iter6>>
set_union(Policy &&policy, Iter1 keys_first1, Iter1 keys_last1,
          Iter2 keys_first2, Iter2 keys_last2, Iter3 values_first1,
          Iter4 values_first2, Iter5 keys_result, Iter6 values_result) {
  auto ret_val = std::set_union(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
      oneapi::dpl::make_zip_iterator(
          keys_last1, values_first1 + std::distance(keys_first1, keys_last1)),
      oneapi::dpl::make_zip_iterator(keys_first2, values_first2),
      oneapi::dpl::make_zip_iterator(
          keys_last2, values_first2 + std::distance(keys_first2, keys_last2)),
      oneapi::dpl::make_zip_iterator(keys_result, values_result),
      internal::compare_key_fun<>());
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_result, values_result), ret_val);
  return std::make_pair(keys_result + n1, values_result + n1);
}

template <class Policy, class Iter1, class Iter2, class Iter3, class Iter4,
          class Iter5, class Iter6, class Comp>
internal::enable_if_execution_policy<Policy, std::pair<Iter5, Iter6>>
set_union(Policy &&policy, Iter1 keys_first1, Iter1 keys_last1,
          Iter2 keys_first2, Iter2 keys_last2, Iter3 values_first1,
          Iter4 values_first2, Iter5 keys_result, Iter6 values_result,
          Comp comp) {
  auto ret_val = std::set_union(
      std::forward<Policy>(policy),
      oneapi::dpl::make_zip_iterator(keys_first1, values_first1),
      oneapi::dpl::make_zip_iterator(
          keys_last1, values_first1 + std::distance(keys_first1, keys_last1)),
      oneapi::dpl::make_zip_iterator(keys_first2, values_first2),
      oneapi::dpl::make_zip_iterator(
          keys_last2, values_first2 + std::distance(keys_first2, keys_last2)),
      oneapi::dpl::make_zip_iterator(keys_result, values_result),
      internal::compare_key_fun<Comp>(comp));
  auto n1 = std::distance(
      oneapi::dpl::make_zip_iterator(keys_result, values_result), ret_val);
  return std::make_pair(keys_result + n1, values_result + n1);
}

template <typename Policy, typename Iter1, typename Iter2, typename Iter3,
          typename Iter4, typename Pred>
internal::enable_if_execution_policy<Policy, std::pair<Iter3, Iter4>>
stable_partition_copy(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask,
                      Iter3 out_true, Iter4 out_false, Pred p) {
  auto ret_val = std::partition_copy(
      std::forward<Policy>(policy), oneapi::dpl::make_zip_iterator(first, mask),
      oneapi::dpl::make_zip_iterator(last, mask + std::distance(first, last)),
      oneapi::dpl::make_zip_iterator(out_true, oneapi::dpl::discard_iterator()),
      oneapi::dpl::make_zip_iterator(out_false,
                                     oneapi::dpl::discard_iterator()),
      internal::predicate_key_fun<Pred>(p));
  return std::make_pair(std::get<0>(ret_val.first.base()),
                        std::get<0>(ret_val.second.base()));
}

template <typename Policy, typename Iter1, typename Iter3, typename Iter4,
          typename Pred>
internal::enable_if_execution_policy<Policy, std::pair<Iter3, Iter4>>
stable_partition_copy(Policy &&policy, Iter1 first, Iter1 last, Iter3 out_true,
                      Iter4 out_false, Pred p) {
  return std::partition_copy(std::forward<Policy>(policy), first, last,
                             out_true, out_false, p);
}

template <typename Policy, typename Iter1, typename Iter2, typename Iter3,
          typename Iter4, typename Pred>
internal::enable_if_execution_policy<Policy, std::pair<Iter3, Iter4>>
partition_copy(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask,
               Iter3 out_true, Iter4 out_false, Pred p) {
  return stable_partition_copy(std::forward<Policy>(policy), first, last, mask,
                               out_true, out_false, p);
}

template <typename Policy, typename Iter1, typename Iter2, typename Pred>
internal::enable_if_execution_policy<Policy, Iter1>
stable_partition(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask, Pred p) {
  typedef typename std::decay<Policy>::type policy_type;
  internal::__buffer<typename std::iterator_traits<Iter1>::value_type> _tmp(
      std::distance(first, last));

  std::copy(std::forward<Policy>(policy), mask,
            mask + std::distance(first, last), _tmp.get());

  auto ret_val =
      std::stable_partition(std::forward<Policy>(policy),
                            oneapi::dpl::make_zip_iterator(first, _tmp.get()),
                            oneapi::dpl::make_zip_iterator(
                                last, _tmp.get() + std::distance(first, last)),
                            internal::predicate_key_fun<Pred>(p));
  return std::get<0>(ret_val.base());
}

template <typename Policy, typename Iter1, typename Iter2, typename Pred>
internal::enable_if_execution_policy<Policy, Iter1>
partition(Policy &&policy, Iter1 first, Iter1 last, Iter2 mask, Pred p) {
  return stable_partition(std::forward<Policy>(policy), first, last, mask, p);
}

} // end namespace dpct

#endif
