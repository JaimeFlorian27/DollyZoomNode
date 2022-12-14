#include <maya/MFnPlugin.h>
#include "dollyZoomNode.h"
#include "dollyZoomCmd.h"

MStatus initializePlugin(MObject mObj) {

	MStatus	  status;
	MFnPlugin plugin(mObj, "Jaime Florian", "1.0", "Any");

	status = plugin.registerNode("dollyZoom", DollyZoomNode::id, DollyZoomNode::creator, DollyZoomNode::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	status = plugin.registerCommand("dollyZoom",DollyZoomCmd::creator, DollyZoomCmd::newSyntax);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;
}

MStatus uninitializePlugin(MObject mObj) {

	MStatus	  status;
	MFnPlugin plugin(mObj);
	status = plugin.deregisterNode(DollyZoomNode::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	status = plugin.deregisterCommand("dollyZoom");
	CHECK_MSTATUS_AND_RETURN_IT(status);
	return status;
}