# maya-directionVelocityNode
An Autodesk Maya C++ dependency graph node that calculates the direction, velocity, and acceleration of an input mesh over time.

**Direction Velocity Node** is designed to accurately compute motion vectors for geometry by evaluating spatial differences across multiple time steps. It utilizes Maya's DG context to look back in time seamlessly, making it highly useful for driving dynamic rigging setups, FX emitters, or procedural animations.

## ✨ Key Features

* **Time-Based Evaluation** (`MDGContext`): Automatically samples mesh positions at previous frames ($t-1$ and $t-2$) to calculate physical distance over time without advancing the scene timeline.
* **Subframe Accuracy:** Features a `subframes` attribute that adjusts the fractional time-step size, preventing division by zero and converting units/subframe into accurate units/frame.
* **Comprehensive Motion Data:** Outputs independent compound attributes for a normalized direction vector, raw velocity, and acceleration (the change in velocity over time).
* **Instant Direction Reversal:** Includes a `reverseDirection` boolean attribute that seamlessly inverts the calculated direction vector without affecting velocity or acceleration magnitudes.

## 🚀 Installation

**Compile:** Build the plugin using CMake and the Maya Developer Kit for your specific Maya version.

**Install:** Place the compiled `directionVelocity.mll` (Windows) / .so (Linux) / `.bundle` (macOS) in a directory mapped to your MAYA_PLUG_IN_PATH.

**Load:** Open Maya's Plug-in Manager and load the plugin.

## 💻 How to Use It

1. Load the plugin in Maya.
2. Open the Maya Script Editor (MEL or Python) to create the node and connect a mesh:

```python
import maya.cmds as cmds

# Create the node
motion_node = cmds.createNode("directionVelocityNode")

# Connect a mesh's world mesh data to the node's input
cmds.connectAttr("pSphereShape1.worldMesh[0]", f"{motion_node}.inputMesh")

# You can now connect the outputs to drive other attributes
cmds.connectAttr(f"{motion_node}.direction", "locator1.translate")
```

## 🎛️ Node Attributes

### Inputs
* **inputMesh (im):** The input mesh data to evaluate.   reverseDirection (rd): Boolean (Default: OFF). Flips the resulting output direction vector.
* **subframes (sbf):** Integer (Default: 1, Min: 1). Determines the step sizing for time evaluation, increasing accuracy for fast-moving objects.

### Outputs
* **direction (dir):** Float3 (dirX, dirY, dirZ). The normalized vector pointing in the direction of travel.
* **velocity (vel):** Float3 (velX, velY, velZ). The spatial distance covered over the evaluated time step.
* **acceleration (acc):** Float3 (accX, accY, accZ). The rate of change in velocity between the current and previous time steps. 

## 🏗️ Architecture / For Developers
* `directionVelocityNode.h` / `directionVelocityNode.cpp`: Defines the core `MPxNode` class that handles the `MDGContext` history evaluations, vector math, and attribute routing.
* `pluginMain.cpp`: The entry point written by David De Juan that handles registering and deregistering the node with the unique ID `0xA7E47158`.   
