// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef EVAL_PARSE_H
#define EVAL_PARSE_H

////////////////////////////////
//~ rjf: Tokenization Functions

#define e_token_at_it(it, arr) (((arr)->v <= (it) && (it) < (arr)->v+(arr)->count) ? (*(it)) : e_token_zero())






////////////////////////////////
//~ rjf: Expression Tree Building Functions









////////////////////////////////
//~ rjf: Expression Tree -> String Conversions




////////////////////////////////
//~ rjf: Parsing Functions








#endif // EVAL_PARSE_H
