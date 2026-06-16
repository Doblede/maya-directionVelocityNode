/**
 * @file pluginMain.cpp
 * @brief Entry point for loading and unloading the DirectionVelocity plugin.
 */

#include "directionVelocityNode.h"
#include <maya/MFnPlugin.h>

/**
 * @brief Registers the plugin with Maya upon loading.
 * @param obj The internal Maya object for the plugin.
 * @return MStatus::kSuccess if the registration succeeds.
 */
MStatus initializePlugin(MObject obj) {
    MStatus status;
    MFnPlugin plugin(obj, "David De Juan", "1.0", "Any");
    
    status = plugin.registerNode(
        "directionVelocityNode", 
        DirectionVelocityNode::id, 
        DirectionVelocityNode::creator, 
        DirectionVelocityNode::initialize
    );
    
    CHECK_MSTATUS_AND_RETURN_IT(status);
    return MS::kSuccess;
}

/**
 * @brief Deregisters the plugin from Maya upon unloading.
 * @param obj The internal Maya object for the plugin.
 * @return MStatus::kSuccess if the deregistration succeeds.
 */
MStatus uninitializePlugin(MObject obj) {
    MStatus status;
    MFnPlugin plugin(obj);
    
    status = plugin.deregisterNode(DirectionVelocityNode::id);
    
    CHECK_MSTATUS_AND_RETURN_IT(status);
    return MS::kSuccess;
}