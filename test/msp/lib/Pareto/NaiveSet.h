#ifndef NAIVE_SET_H
#define NAIVE_SET_H


#include <unordered_set>
#include "Set.h"

namespace pareto
{

class NaiveSet : public Set
{
public:
	void insert(sp::Paths paths) override;

private:
	virtual void insert(sp::PathPtr path);
	typedef sp::PathPtr elem_t;
	std::unordered_set<elem_t> m_set;
};

}

#endif // NAIVE_SET_H
