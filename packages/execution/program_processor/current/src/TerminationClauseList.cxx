#include "execution/TerminationClauseList.hpp"
#include "execution/ProgramNode.hpp"

namespace ORO_Execution
{

	TerminationClauseList::TerminationClauseList()
          : clauses( new std::vector<TerminationClause*> )
	{
	}

	ProgramNode* TerminationClauseList::evaluate()
	{
		ProgramNode* result = 0 ;

		TerminationClause* c = check();

		if (c != 0 )
		{
			//TerminationClause* t = (TerminationClause*) c;
			result = c->getProgramNode();

		}

		return result;

	}


	TerminationClauseList::TerminationClauseList(TerminationClause* clause)
	{
		clauses->push_back(clause);
	}

	void TerminationClauseList::addClause(TerminationClause* clause)
	{
		clauses->push_back(clause);
	}

	TerminationClause* TerminationClauseList::check()
	{

		typedef std::vector<TerminationClause*>::iterator it;

		it ptr = clauses->begin();

		bool done=false;

		while(!done && ptr != clauses->end())
		{
			done = (*ptr)->evaluate();
			++ptr;
		}
		//TODO
		if (done){ --ptr; return *ptr;}
		else {return 0;}

		//hier komt ie ni
		return 0;
	}

  namespace {
    template<typename iter>
    void delete_all( iter begin, iter end )
    {
      for ( ; begin != end; ++begin )
        delete *begin;
    }
  }

TerminationClauseList::~TerminationClauseList()
{
  delete_all( clauses->begin(), clauses->end() );
  delete clauses;
}

	int TerminationClauseList::size()
	{
		size_t result=clauses->size();
		return (int)result;
	}

	void TerminationClauseList::reset()
	{
		typedef std::vector<TerminationClause*>::iterator it;
                for ( it i = clauses->begin(); i != clauses->end(); ++i )
                        (*i)->reset();
	}


}

