
#ifndef PARETOLOCALITYTASKSTORAGEITEMREUSECHECK_H_
#define PARETOLOCALITYTASKSTORAGEITEMREUSECHECK_H_

namespace pheet
{

template <class Item>
struct ParetoLocalityTaskStorageItemReuseCheck {
	bool operator()(Item const&) const
	{
		return false; //TODO: this is a dummy implementation
	}
};

} /* namespace pheet */
#endif /* PARETOLOCALITYTASKSTORAGEITEMREUSECHECK_H_*/
