#ifndef _EVALUATOR_T_
#define _EVALUATOR_T_

#include "boolean.h"
#include "environment.h"
#include "tagged-reference.h"

extern tagged_reference_t eval(environment_t* env, tagged_reference_t expr,
                               boolean_t in_tail_position);

#endif /* _EVALUATOR_T_ */
