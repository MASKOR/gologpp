:- external(exog_fluent_getValue/3, p_exog_fluent_getValue).
:- dynamic managed_term/2.
:- dynamic durative_action/1, durative_poss/2, durative_causes_val/4.
:- lib(listut).

% resolve name clash with lib(listut)
:- import delete/3 from eclipse_language.

function(strcat(X, Y), R, concat_atoms(X, Y, R)).
function(to_string(V), R,
	and(sprintf(S, "%w", V), atom_string(R, S))
).


function(gpp_field_value(Name, Compound), Value,
	once(pl_field_value(Name, Compound, Value))
).
pl_field_value(Name, Compound, Value) :-
	(
		atom(Name), var(Value)
		, Compound = gpp_compound(_Type, Fields)
		, Field_term =.. [Name, Value]
		, member(Field_term, Fields)
	;
		sprintf(Msg, "Invalid function call: %W", [gpp_field_value(Name, Compound)])
		, throw(Msg)
	)
.


function(gpp_mixed_assign(Members, Value, Lhs), Result,
	once(pl_mixed_assign(Members, Value, Lhs, Result))
).
pl_mixed_assign(Members, Value, Lhs, Result) :-
	Members = [M|Rest_mems]
	, (number(M) ->
		% List access
		( Lhs = gpp_list(Type, List), length(List, _)
			; sprintf(Msg, "gpp_mixed_assign: Invalid argument %W", [gpp_mixed_assign(Members, Value, Lhs)])
			, throw(Msg)
		)
		, ( nth0(M, List, Mem_current, List_without)
			; sprintf(Msg, "Accessing %W: Index %W is out of bounds", [List, M])
			, throw(Msg)
		)
		, ( length(Rest_mems) > 0 ->
			% Additional nesting levels: recurse down
			pl_mixed_assign(Rest_mems, Value, Mem_current, Val_new)
		;
			Val_new = Value
		)
		, nth0(M, List_new, Val_new, List_without)
		, Result = gpp_list(Type, List_new)
	
	; atom(M) ->
		% Compound field access
		( Lhs = gpp_compound(Type, Fields), length(Fields, _)
			; sprintf(Msg, "gpp_mixed_assign: Invalid argument %W", [gpp_mixed_assign(Members, Value, Lhs)])
			, throw(Msg)
		)
		, Field_acc =.. [M, Field_cur_value]
		, delete(Field_acc, Fields, Fields_without)
		, ( length(Rest_mems) > 0 ->
			% Additional nesting levels: recurse down
			pl_mixed_assign(Rest_mems, Value, Field_cur_value, Val_new)
			, Field_new =.. [M, Val_new]
		;
			Field_new =.. [M, Value]
		)
		, Result = gpp_compound(Type, [Field_new | Fields_without])
	;
		sprintf(Msg, "gpp_mixed_assign: %W is not a list index or a field name", [M])
		, throw(Msg)
	)
.

function(gpp_list_access(Lhs, Idx), Result,
	once(pl_list_access(Lhs, Idx, Result))
).
pl_list_access(Lhs, Idx, Result) :-
	(
		Lhs = gpp_list(_Type, List)
		, length(List, _)
		, number(Idx)
	;
		sprintf(Msg, "Invalid function call: %W", [pl_list_access(Lhs, Idx, Result)])
		, throw(Msg)
	)
	, nth0(Idx, List, Result)
.

function(gpp_list_length(Lhs), Result,
	once(pl_list_length(Lhs, Result))
).
pl_list_length(Lhs, Result) :-
	(
		Lhs = gpp_list(_Type, List)
		, length(List, Result)
	;
		sprintf(Msg, "Invalid function call: %W", [pl_list_length(Lhs, Result)])
		, throw(Msg)
	)
.

function(gpp_list_pop_front(Lhs), Result,
	once(pl_list_pop_front(Lhs, Result))
).
pl_list_pop_front(Lhs, Result) :-
	(
		Lhs = gpp_list(Type, List)
		, length(List, _)
		; throw("gpp_list_pop_front: Arg must be a list")
	)
	, List_new = [_ | Result]
	, Result = gpp_list(Type, List_new)
.

function(gpp_list_pop_back(Lhs), Result,
	once(pl_list_pop_back(Lhs, Result))
).
pl_list_pop_back(Lhs, Result) :-
	(
		Lhs = gpp_list(Type, List)
		, length(List, _)
		; throw("gpp_list_pop_back: Arg must be a list")
	)
	, length(List, Len)
	, nth1(Len, List, _, List_new)
	, Result = gpp_list(Type, List_new)
.

function(gpp_list_push_front(Lhs, Elem), Result,
	once(pl_list_push_front(Lhs, Elem, Result))
).
pl_list_push_front(Lhs, Elem, Result) :-
	(
		Lhs = gpp_list(Type, List)
		, length(List, _)
		; throw("gpp_list_push_front: First arg must be a list")
	)
	, ( number(Elem)
		; throw("gpp_list_push_front: Second arg must be instantiated")
	)
	, Result = gpp_list(Type, [Elem | List])
.

function(gpp_list_push_back(Lhs, Elem), Result,
	once(pl_list_push_back(Lhs, Elem, Result))
).
pl_list_push_back(Lhs, Elem, Result) :-
	(
		Lhs = gpp_list(Type, List)
		, length(List, _)
		; throw("gpp_list_push_back: First arg must be a list")
	)
	, ( number(Elem)
		; throw("gpp_list_push_back: Second arg must be instantiated")
	)
	, append(List, [Elem], List_new)
	, Result = gpp_list(Type, List_new)
.


/********************************
 * Durative Action semantics
 * ******************************/

exog_prim_fluent(now).

prim_fluent(state(A)) :- durative_action(A).
initial_val(state(A), idle) :- durative_action(A).
reserved_fluent(state(_A)).

exog_action(exog_state_change(A, _T, State)) :- durative_action(A).
poss(exog_state_change(A, _T, State), true) :- durative_action(A).
causes_val(exog_state_change(A, _T, State), state(A), State, true) :- durative_action(A).


% start action
% ============
prim_action(start(A, _T)) :- durative_action(A).

% Precondition only applies to starting a durative action
poss(start(A, T),
	and([
		not(state(A) = running)
		, now >= T
		, C
	])
) :-
	durative_action(A)
	, durative_poss(A, C)
.
causes_val(start(A, _T), state(A), running, true) :- durative_action(A).


% finish action
% ============
prim_action(finish(A, _T)) :- durative_action(A).
poss(finish(A, T),
	and(
		lif(online = true
			% Online: Wait for real action to complete
			, state(A) = final
			% Offline: Final immediately
			, true
		)
		, now >= T
	)
) :- durative_action(A).
causes_val(finish(A, _T), state(A), final, true) :- durative_action(A).

% Only the finish action applies a durative action's effects:
causes_val(finish(A, T), F, V, C) :- durative_causes_val(A, F, V, C).


% end action
% ==========
prim_action(end(A, _T)) :- durative_action(A).
poss(end(A, T),
	and(
		lif(online
			% Online: Wait for real action to complete
			, or( [
				state(A) = final
				, state(A) = failed
				, state(A) = cancelled
			] )
			% Offline: Final immediately
			, true
		)
		, now >= T
	)
) :- durative_action(A).

% ONLY During planning end(A) puts A in the final state.
% Online this happens exogenously.
causes_val(end(A, _T), state(A), final, not(online)) :- durative_action(A).

% Offline: Just apply effect & disregard state because state is changed
%          by another effect.
% Online:  Apply effect only if state is final
causes_val(
	end(A, _T)
	, F, V
	, lif(online
		, and(state(A) = final, C)
		, C
	)
) :-
	durative_causes_val(A, F, V, C)
.


% stop action
% ===========
prim_action(stop(A, _T)) :- durative_action(A).
poss(stop(A, T),
	and(
		lif(online = true
			, state(A) = running
			, true
		)
		, now >= T
	)
) :- durative_action(A).
causes_val(stop(A, _T), state(A), cancelled, true) :- durative_action(A).


% fail action
% =============
prim_action(fail(A, _T)) :- durative_action(A).
poss(fail(A, T),
	and([
		online = true
		, state(A) = failed
		, now >= T
	])
) :- durative_action(A).
causes_val(fail(A, _T), state(A), failed, true) :- durative_action(A).



/********************************/


ssa( online, false, [clipOnline|_69485] ) :- !, true.
ssa( online, true, [setOnline|_69524] ) :- !, true.
ssa( sit_start_time, T, [setTime(T)|_69563] ) :- !, true.
ssa( sit_start_time, T, [ccUpdate(_69409, T)|_69585] ) :- !, true.
ssa( Fluent, Value, [set(Fluent, Value)|_69607] ) :- !, true.

ssa( eval_exog_functions, _69895, [set(eval_exog_functions, _69895)|_] ) :- !.
ssa( eval_registers, _69906, [set(eval_registers, _69906)|_] ) :- !.
ssa( online, _69917, [set(online, _69917)|_] ) :- !.
ssa( sit_start_time, _69928, [set(sit_start_time, _69928)|_] ) :- !.
ssa( useAbstraction, _69939, [set(useAbstraction, _69939)|_] ) :- !.
ssa( bel(_69762), _69950, [set(bel(_69762), _69950)|_] ) :- !.
ssa( ltp(_69766), _69961, [set(ltp(_69766), _69961)|_] ) :- !.

ssa( lookahead(_69755, _69756, _69757, _69758), _70148, [set(lookahead(_69755, _69756, _69757, _69758), _70148)|_] ) :- !.
ssa( pll(_69743, _69744, _69745, _69746), _70159, [set(pll(_69743, _69744, _69745, _69746), _70159)|_] ) :- !.
ssa( eval_exog_functions, _70170, [_70174|_70175] ) :- !, has_val(eval_exog_functions, _70170, _70175).
ssa( eval_registers, _70190, [_70194|_70195] ) :- !, has_val(eval_registers, _70190, _70195).
ssa( online, _70210, [_70214|_70215] ) :- !, has_val(online, _70210, _70215).
ssa( sit_start_time, _70230, [_70234|_70235] ) :- !, has_val(sit_start_time, _70230, _70235).
ssa( useAbstraction, _70250, [_70254|_70255] ) :- !, has_val(useAbstraction, _70250, _70255).
ssa( bel(_69762), _70270, [_70274|_70275] ) :- !, has_val(bel(_69762), _70270, _70275).
ssa( ltp(_69766), _70290, [_70294|_70295] ) :- !, has_val(ltp(_69766), _70290, _70295).

ssa( lookahead(_69755, _69756, _69757, _69758), _70630, [_70634|_70635] ) :- !, has_val(lookahead(_69755, _69756, _69757, _69758), _70630, _70635).
ssa( pll(_69743, _69744, _69745, _69746), _70650, [_70654|_70655] ) :- !, has_val(pll(_69743, _69744, _69745, _69746), _70650, _70655).

prolog_poss( send(_R, _70692) ).
prolog_poss( send(_R, _70692), _71116 ) :- true.
prolog_poss( reply(_R, _70701) ).
prolog_poss( reply(_R, _70701), _71133 ) :- true.
prolog_poss( setOnline ).
prolog_poss( setOnline, _71150 ) :- true.
prolog_poss( clipOnline ).
prolog_poss( clipOnline, _71167 ) :- true.
prolog_poss( setTime(_70719) ).
prolog_poss( setTime(_70719), _71184 ) :- true.
prolog_poss( clipAbstraction ).
prolog_poss( clipAbstraction, _71201 ) :- true.
prolog_poss( setAbstraction ).
prolog_poss( setAbstraction, _71218 ) :- true.
prolog_poss( set(_Fluent, _Value) ).
prolog_poss( set(_Fluent, _Value), _71235 ) :- true.
prolog_poss( exogf_Update ).
prolog_poss( exogf_Update, _71252 ) :- true.

