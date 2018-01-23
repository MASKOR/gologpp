#ifndef READYLOG_UTILITIES_H_
#define READYLOG_UTILITIES_H_

#include <gologpp/utilities.h>
#include <gologpp/Translation.h>
#include <gologpp/Reference.h>
#include <eclipseclass.h>


namespace gologpp {
namespace readylog {

using namespace std;


template<class GologT>
class Reference : public Translatable<generic::Reference<GologT>, EC_word> {
public:
	using Translatable<generic::Reference<GologT>, EC_word>::Translatable;

private:
	void init(EC_word &cache) {

	}
};


} // namespace readylog
} // namespace gologpp



#endif // READYLOG_UTILITIES_H_