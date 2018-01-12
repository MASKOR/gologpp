#ifndef GOLOGPP_LANGUAGE_H_
#define GOLOGPP_LANGUAGE_H_

#include <tuple>

namespace gologpp {

using namespace std;

template<class... MemberTs>
class LanguageElement {
public:
	LanguageElement(MemberTs... members)
	: members_(members...)
	{}

	typedef tuple<MemberTs...> members_type;
protected:
	members_type members_;
};

}

#endif // GOLOGPP_LANGUAGE_H_