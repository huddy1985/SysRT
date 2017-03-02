/***************************************************************************
Author: Jun Xiao
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __DAGNODE_HPP__
#define __DAGNODE_HPP__

#include "rttask.hpp"
#include <vector>


class DagNode: public PeriodicTask {
public:
	std::vector<DagNode *> parents;
	std::vector<DagNode *> children;
	uint64_t job_index;
public:
	DagNode(sc_time iat, sc_time rdl, sc_time ph, sc_module_name name = "",
			long qs = 100);
	~DagNode(){;};
	void AddParentTaskOnly(DagNode* parent);
	void AddChildrenTaskOnly(DagNode* child);
	void AddChildTask(DagNode* child) ;
	void AddParentTask(DagNode* parent) ;

	bool isSourceTask() ;
	bool isSinkTask() ;
	bool check_parents_job() ;
	void activate_chilren();

	void newRun(void);
//remeber to set it virtual
	void onEndInstance();
	void activate();
    void onArrival();

};
#endif
