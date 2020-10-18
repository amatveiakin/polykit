import math
import re

from tensor import Inf
from linear import Annotation, Linear
from lyndon import to_lyndon_basis
from polylog_gen import Li
from enumerator import Enumerator
from util import generate_all_words, get_one_item
from word_algebra import word_index_map_to_annotation_index_map, word_expr_max_char, words_with_n_distinct_chars, word_expr_to_template, word_expr_substitute, annotation_expr_substitute


def is_representative_indices(indices):
    max_seen = 0  # Allow both 0 and 1 at the first position, because 0 has special meaning
    for idx in indices:
        if idx > max_seen + 1:
            return False
        max_seen = idx
    return True

# TODO: Only gen indices > projection_exis
def _index_after_projection(i, projection_axis):
    return i + (0 if i < projection_axis else 1)

# def apply_eliminations(...):
#     ret = Linear()
#     for word, coeff in expression.items():
#         if not word_to_template(word) in known_eliminations:
#             ret[word] = coeff
#     return ret

def eliminate(result, result_annotations, elimination, elimination_annotations, min_distinct=0):
    while (
        words_with_n_distinct_chars(result - elimination, min_distinct).l1_norm() <
        words_with_n_distinct_chars(result, min_distinct).l1_norm()
    ):
        result -= elimination
        result_annotations -= elimination_annotations
    while (
        words_with_n_distinct_chars(result + elimination, min_distinct).l1_norm() <
        words_with_n_distinct_chars(result, min_distinct).l1_norm()
    ):
        result += elimination
        result_annotations += elimination_annotations

def apply_eliminations(known_eliminations, expression, projection_axis, min_distinct=0):
    ret = expression.without_annotations()
    if ret == Linear():
        return ret
    source_points = max(word_expr_max_char(expression), projection_axis)
    max_elimination_points = 0
    for elim, _ in known_eliminations.values():
        max_elimination_points = max(max_elimination_points, word_expr_max_char(elim)+1)
    index_combinations = generate_all_words(alphabet_size=source_points-1, length=max_elimination_points)
    for indices in index_combinations:
        index_map = {
            i: _index_after_projection(indices[i] + 1, projection_axis)
            for i in range(len(indices))
        }
        for elim, elim_annotations in known_eliminations.values():
            elim_mapped = word_expr_substitute(elim, index_map)
            elim_annotations_mapped = annotation_expr_substitute(
                elim_annotations,
                word_index_map_to_annotation_index_map(index_map, projection_axis, kind="Inverse")
            )
            eliminate(ret, ret, elim_mapped, elim_annotations_mapped, min_distinct=min_distinct)
    return ret

def _normalized_linear(expr):
    divisor = None
    for _, coeff in expr.items():
        k = abs(coeff)
        assert k > 0, expr
        if divisor is None:
            divisor = k
        else:
            divisor = math.gcd(divisor, k)
    sign = 1 if get_one_item(expr.items())[1] > 0 else -1
    return expr.div_int(divisor * sign)

def _add_elimination_impl(known_eliminations, expression, annotations, projection_axis):
    index_map = {}
    tmpl = word_expr_to_template(_normalized_linear(expression), index_map)
    tmpl_key = str(tmpl)
    if not tmpl_key in known_eliminations:
        # print(f"Found template: {tmpl}")
        known_eliminations[tmpl_key] = (
            tmpl,
            annotation_expr_substitute(
                annotations,
                word_index_map_to_annotation_index_map(index_map, projection_axis, kind="Direct")
            )
        )

def add_elimination(known_eliminations, expression, projection_axis):
    _add_elimination_impl(
        known_eliminations,
        to_lyndon_basis(expression.without_annotations()),
        expression.annotations(),
        projection_axis,
    )

# TODO: auto-compute num_points and weight
def find_eliminations(known_eliminations, expression, num_points, weight, projection_axis, min_distinct=0):
    index_combinations = generate_all_words(alphabet_size=num_points, length=weight)
    annotations = expression.annotations()
    expr = to_lyndon_basis(expression.without_annotations())
    for indices in index_combinations:
        if not is_representative_indices(indices):
            continue
        index_map = {
            _index_after_projection(i + 1, projection_axis):
                Inf if indices[i] == 0 else _index_after_projection(indices[i], projection_axis)
            for i in range(len(indices))
        }
        # print("indices = (" + ", ".join(map(str, index_map.values())) + ")")
        words_new = to_lyndon_basis(word_expr_substitute(expr, index_map))
        # if len(words_new) > 0:
        #     print("Li5_p6(" + ", ".join(map(str, index_map.values())) + ") =\n" + str(words_new))
        words_distinct = words_with_n_distinct_chars(words_new, min_distinct)
        if 0 < len(words_distinct) and len(words_distinct) <= 4:
            _add_elimination_impl(
                known_eliminations,
                words_new,
                annotation_expr_substitute(
                    annotations,
                    word_index_map_to_annotation_index_map(index_map, projection_axis, kind="Direct")
                ),
                projection_axis
            )

def str_to_arg(s):
    return Inf if s == "Inf" else int(s)

def annotation_to_func(annotation):
    assert isinstance(annotation, Annotation)
    m = re.match(r"Li(.*)\((.*)\)", annotation.name)
    ret = Li(int(m.group(1)), [str_to_arg(a) for a in m.group(2).split(",")])
    return ret
    # return Linear() if to_lyndon_basis(ret.without_annotations()) == Linear() else ret
