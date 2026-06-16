/**
 * @file directionVelocityNode.h
 * @brief Defines a Maya custom dependency graph node for calculating direction, velocity, and acceleration.
 * @author David De Juan
 */

#ifndef DIRECTIONVELOCITYNODE_H
#define DIRECTIONVELOCITYNODE_H

#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>

#ifdef _DEBUG
    #define DEBUG_LOG(msg) \
        { \
            MString dbg = "[directionVelocityNode DEBUG] "; \
            dbg += msg; \
            MGlobal::displayInfo(dbg); \
        }
#else
    #define DEBUG_LOG(msg) 
#endif

class MPlug;
class MDataBlock;

/**
 * @class DirectionVelocityNode
 * @brief Computes the directional vector, velocity, and acceleration of an input mesh over time.
 */
class DirectionVelocityNode : public MPxNode {
public:
    DirectionVelocityNode();
    virtual ~DirectionVelocityNode();

    virtual MStatus compute(const MPlug& plug, MDataBlock& data) override;
    static void* creator();
    static MStatus initialize();

public:
    // Node Identity
    static MTypeId id;

    // Input Attributes
    static MObject aInput;      ///< Input mesh data
    static MObject aReverse;    ///< Boolean to reverse the output direction vector
    static MObject aSubframes;  ///< Number of subframes for evaluation accuracy

    // Output Attributes (Direction)
    static MObject aDirX;
    static MObject aDirY;
    static MObject aDirZ;
    static MObject aOutDirection;

    // Output Attributes (Velocity)
    static MObject aVelX;
    static MObject aVelY;
    static MObject aVelZ;
    static MObject aOutVelocity;

    // Output Attributes (Acceleration)
    static MObject aAccX;
    static MObject aAccY;
    static MObject aAccZ;
    static MObject aOutAcceleration;
};

#endif // DIRECTIONVELOCITYNODE_H