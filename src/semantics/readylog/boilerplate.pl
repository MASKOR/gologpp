:- external(exog_fluent_getValue/3, p_exog_fluent_getValue).
:- dynamic managed_term/2.
:- dynamic durative_action/1, durative_poss/2, durative_causes_val/4.

function(strcat(X, Y), R, concat_atoms(X, Y, R)).
function(to_string(V), R,
	and(sprintf(S, "%w", V), atom_string(R, S))
).


function(gpp_field_value(Name, gpp_compound(Fields)), Value,
	and([
		atom(Name), var(Value)
		, Field_term =.. [Name, Value]
		, member(Field_term, Fields)
	])
).

function(gpp_field_assign(Names, Value, gpp_compound(Fields)), Result,
	and([
		Names = [Name|Rest_names]
		, atom(Name), var(Result), ground(Value)
		, Field_current =.. [Name, Field_cur_value]
		, delete(Field_current, Fields, Fields_without)
		,
		lif(Field_cur_value = gpp_compound(_),
			% Nested compound, so recurse down
			% hoping the length of Names matches the nesting depth
			Field_new =.. [Name, gpp_field_assign(Rest_names, Value, Field_cur_value)]
		,
			Field_new =.. [Name, Value]
		)
		, Result = gpp_compound([Field_new | Fields_without])
	])
).


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
causes_val(stop(A, _T), state(A), preempted, true) :- durative_action(A).


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

