#include <eclipseclass.h>

int main(int argc, char **argv) {
	ec_set_option_ptr(EC_OPTION_ECLIPSEDIR, const_cast<void *>(static_cast<const void *>(ECLIPSE_DIR)));
	ec_init();
	EC_word t1, t2, t3;
	EC_word v1, v2, v3;

	v1 = ::newvar();
	t1 = ::term(EC_functor("writeln", 1), v1);

	::post_goal("garbage_collect");
	EC_resume();

	v2 = ::newvar();
	t2 = ::term(EC_functor("writeln", 1), v2);


	::post_goal("garbage_collect");
	EC_resume();

	v3 = ::newvar();
	t3 = ::term(EC_functor("writeln", 1), v3);

	::post_goal("garbage_collect");
	EC_resume();

	::post_goal(t1);
	EC_resume();

	::post_goal(t2);
	EC_resume();

	::post_goal(t3);
	EC_resume();














}
