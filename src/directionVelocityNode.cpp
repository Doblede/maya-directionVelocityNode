/**
 * @file directionVelocityNode.cpp
 * @brief Implementation of the DirectionVelocityNode class.
 */

#include "directionVelocityNode.h"

// Maya API Includes
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMeshData.h>
#include <maya/MDataHandle.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MAnimControl.h>
#include <maya/MDGContext.h>
#include <maya/MDGContextGuard.h>

// Unique Node ID
MTypeId DirectionVelocityNode::id(0xA7E47158);

// Attribute Initialization
MObject DirectionVelocityNode::aInput;
MObject DirectionVelocityNode::aReverse;
MObject DirectionVelocityNode::aSubframes;

MObject DirectionVelocityNode::aDirX;
MObject DirectionVelocityNode::aDirY;
MObject DirectionVelocityNode::aDirZ;
MObject DirectionVelocityNode::aOutDirection;

MObject DirectionVelocityNode::aVelX;
MObject DirectionVelocityNode::aVelY;
MObject DirectionVelocityNode::aVelZ;
MObject DirectionVelocityNode::aOutVelocity;

MObject DirectionVelocityNode::aAccX;
MObject DirectionVelocityNode::aAccY;
MObject DirectionVelocityNode::aAccZ;
MObject DirectionVelocityNode::aOutAcceleration;


DirectionVelocityNode::DirectionVelocityNode() {}
DirectionVelocityNode::~DirectionVelocityNode() {}

MStatus DirectionVelocityNode::compute(const MPlug& plug, MDataBlock& data) {
    if (plug != aOutDirection && plug != aDirX && plug != aDirY && plug != aDirZ &&
        plug != aOutVelocity && plug != aVelX && plug != aVelY && plug != aVelZ &&
        plug != aOutAcceleration && plug != aAccX && plug != aAccY && plug != aAccZ) {
        return MS::kUnknownParameter;
    }

    // Determine subframe step sizing
    int subframes = data.inputValue(aSubframes).asInt();
    if (subframes < 1) subframes = 1; // Prevent division by zero
    double step = 1.0 / static_cast<double>(subframes);

    // 1. Get position at t0 (Current Frame)
    MDataHandle inputMeshHandle = data.inputValue(aInput);
    MVector p0 = MTransformationMatrix(inputMeshHandle.geometryTransformMatrix()).getTranslation(MSpace::kWorld);

    // 2. Get position at t1 (t0 - 1 step)
    MDGContext ctx1(MAnimControl::currentTime() - step);
    MVector p1;
    {
        MDGContextGuard guard(ctx1);
        p1 = MTransformationMatrix(data.inputValue(aInput).geometryTransformMatrix()).getTranslation(MSpace::kWorld);
    }

    // 3. Get position at t2 (t0 - 2 steps)
    MDGContext ctx2(MAnimControl::currentTime() - (2.0 * step));
    MVector p2;
    {
        MDGContextGuard guard(ctx2);
        p2 = MTransformationMatrix(data.inputValue(aInput).geometryTransformMatrix()).getTranslation(MSpace::kWorld);
    }

    // 4. Vector calculations
    // Velocity is distance over time. Multiplying by 'subframes' converts units/subframe to units/frame.
    MVector velocity = (p0 - p1) * subframes;
    MVector oldVelocity = (p1 - p2) * subframes;
    
    // Acceleration is the change in velocity over time.
    MVector acceleration = (velocity - oldVelocity) * subframes;
    
    // Direction calculation and reversal check
    MVector direction = velocity.normal();
    if (data.inputValue(aReverse).asBool()) {
        direction = -direction;
    }

    // 5. Set the output data
    data.outputValue(aDirX).setFloat(static_cast<float>(direction.x));
    data.outputValue(aDirY).setFloat(static_cast<float>(direction.y));
    data.outputValue(aDirZ).setFloat(static_cast<float>(direction.z));
    
    data.outputValue(aVelX).setFloat(static_cast<float>(velocity.x));
    data.outputValue(aVelY).setFloat(static_cast<float>(velocity.y));
    data.outputValue(aVelZ).setFloat(static_cast<float>(velocity.z));

    data.outputValue(aAccX).setFloat(static_cast<float>(acceleration.x));
    data.outputValue(aAccY).setFloat(static_cast<float>(acceleration.y));
    data.outputValue(aAccZ).setFloat(static_cast<float>(acceleration.z));
    
    data.setClean(plug);
    return MS::kSuccess;
}

void* DirectionVelocityNode::creator() {
    return new DirectionVelocityNode();
}

MStatus DirectionVelocityNode::initialize() {
    MFnNumericAttribute nAttr;
    MFnTypedAttribute tAttr;
    MFnCompoundAttribute cAttr;

    // --- Input Attributes ---
    aInput = tAttr.create("inputMesh", "im", MFnMeshData::kMesh);
    tAttr.setWritable(true);
    tAttr.setReadable(false);
    tAttr.setStorable(false);
    tAttr.setHidden(false);
    CHECK_MSTATUS(addAttribute(aInput));

    aReverse = nAttr.create("reverseDirection", "rd", MFnNumericData::kBoolean, 0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setChannelBox(true);
    CHECK_MSTATUS(addAttribute(aReverse));

    aSubframes = nAttr.create("subframes", "sbf", MFnNumericData::kInt, 1);
    nAttr.setMin(1);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setChannelBox(true);
    CHECK_MSTATUS(addAttribute(aSubframes));


    // --- Output Attributes: Direction ---
    aDirX = nAttr.create("dirX", "dx", MFnNumericData::kFloat);
    aDirY = nAttr.create("dirY", "dy", MFnNumericData::kFloat);
    aDirZ = nAttr.create("dirZ", "dz", MFnNumericData::kFloat);
    nAttr.setReadable(true); nAttr.setWritable(false); nAttr.setStorable(false);

    aOutDirection = cAttr.create("direction", "dir");
    cAttr.addChild(aDirX); cAttr.addChild(aDirY); cAttr.addChild(aDirZ);
    cAttr.setReadable(true); cAttr.setWritable(false);
    CHECK_MSTATUS(addAttribute(aOutDirection));


    // --- Output Attributes: Velocity ---
    aVelX = nAttr.create("velX", "vx", MFnNumericData::kFloat);
    aVelY = nAttr.create("velY", "vy", MFnNumericData::kFloat);
    aVelZ = nAttr.create("velZ", "vz", MFnNumericData::kFloat);
    nAttr.setReadable(true); nAttr.setWritable(false); nAttr.setStorable(false);

    aOutVelocity = cAttr.create("velocity", "vel");
    cAttr.addChild(aVelX); cAttr.addChild(aVelY); cAttr.addChild(aVelZ);
    cAttr.setReadable(true); cAttr.setWritable(false);
    CHECK_MSTATUS(addAttribute(aOutVelocity));


    // --- Output Attributes: Acceleration ---
    aAccX = nAttr.create("accX", "ax", MFnNumericData::kFloat);
    aAccY = nAttr.create("accY", "ay", MFnNumericData::kFloat);
    aAccZ = nAttr.create("accZ", "az", MFnNumericData::kFloat);
    nAttr.setReadable(true); nAttr.setWritable(false); nAttr.setStorable(false);

    aOutAcceleration = cAttr.create("acceleration", "acc");
    cAttr.addChild(aAccX); cAttr.addChild(aAccY); cAttr.addChild(aAccZ);
    cAttr.setReadable(true); cAttr.setWritable(false);
    CHECK_MSTATUS(addAttribute(aOutAcceleration));


    // --- Attribute Affects Setup ---
    MObject outputs[] = { aDirX, aDirY, aDirZ, aOutDirection, 
                          aVelX, aVelY, aVelZ, aOutVelocity,
                          aAccX, aAccY, aAccZ, aOutAcceleration };

    // Mesh Input affects everything
    for (MObject out : outputs) {
        CHECK_MSTATUS(attributeAffects(aInput, out));
    }

    // Subframes affects everything (changes the time-step)
    for (MObject out : outputs) {
        CHECK_MSTATUS(attributeAffects(aSubframes, out));
    }

    // Reverse boolean ONLY affects Direction outputs
    CHECK_MSTATUS(attributeAffects(aReverse, aDirX));
    CHECK_MSTATUS(attributeAffects(aReverse, aDirY));
    CHECK_MSTATUS(attributeAffects(aReverse, aDirZ));
    CHECK_MSTATUS(attributeAffects(aReverse, aOutDirection));

    return MS::kSuccess;
}