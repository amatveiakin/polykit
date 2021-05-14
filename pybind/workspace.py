from polykit import Encoder, RichTextFormat, AnnotationSorting, set_formatting, reset_formatting, NoLineLimit
from polykit import tensor_product, to_lyndon_basis
from polykit import coproduct, comultiply
from polykit import X, Inf, Delta, DeltaExpr, substitute_variables
from polykit import ProjectionExpr, project_on, involute, involute_projected
from polykit import terms_with_num_distinct_variables, terms_with_min_distinct_variables, terms_containing_only_variables, terms_without_variables
from polykit import sorted_by_num_distinct_variables
from polykit import CrossRatio, CompoundRatio, CR
from polykit import I, Corr
from polykit import QLi, QLi1, QLi2, QLi3, QLi4, QLi5, QLi6, QLi7, QLi8
from polykit import QLiNeg, QLiNeg1, QLiNeg2, QLiNeg3, QLiNeg4, QLiNeg5, QLiNeg6, QLiNeg7, QLiNeg8
from polykit import QLiSymm, QLiSymm1, QLiSymm2, QLiSymm3, QLiSymm4, QLiSymm5, QLiSymm6, QLiSymm7, QLiSymm8
from polykit import QLiPr
from polykit import Lira, Lira0, Lira1, Lira2, Lira3, Lira4, Lira5, Lira6, Lira7, Lira8
from polykit import project_on, project_on_x1, project_on_x2, project_on_x3, project_on_x4, project_on_x5, project_on_x6, project_on_x7, project_on_x8, project_on_x9, project_on_x10, project_on_x11, project_on_x12, project_on_x13, project_on_x14, project_on_x15


# set_formatting(encoder=Encoder.unicode)

print(to_lyndon_basis(QLi2(1,2,3,4)))
