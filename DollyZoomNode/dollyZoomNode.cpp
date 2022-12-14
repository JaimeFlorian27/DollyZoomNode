#include "dollyZoomNode.h"
#include <iostream>


MTypeId DollyZoomNode::id(0x0013b880);
MObject DollyZoomNode::aCameraHorizontalAperture;
MObject DollyZoomNode::aOutputAperture;
MObject DollyZoomNode::aCameraWorldMatrix;
MObject DollyZoomNode::aTargetWorldMatrix;
MObject DollyZoomNode::aWidth;
MObject DollyZoomNode::aOutputFocalLength;
MObject DollyZoomNode::aDistance;

DollyZoomNode::DollyZoomNode()
{
}
DollyZoomNode::~DollyZoomNode()
{
}
void* DollyZoomNode::creator()
{
    return new DollyZoomNode();
}

MStatus DollyZoomNode::initialize()
{
    MStatus status(MS::kSuccess);

    //numeric Attrbituies

    MFnNumericAttribute numericAttr;

    aWidth = numericAttr.create("width", "width", MFnNumericData::kDouble);
    numericAttr.setMin(0.0001);
    numericAttr.setDefault(4.0);
    numericAttr.setKeyable(true);
    addAttribute(aWidth);


    aCameraHorizontalAperture = numericAttr.create("horizontalAperture", "horizontalAperture", MFnNumericData::kDouble);
    numericAttr.setMin(0.0001);
    numericAttr.setDefault(36.0);
    numericAttr.setKeyable(true);
    addAttribute(aCameraHorizontalAperture);


    aDistance = numericAttr.create("distance", "distance", MFnNumericData::kDouble);
    numericAttr.setWritable(false);
    numericAttr.setStorable(false);
    addAttribute(aDistance);

    aOutputFocalLength = numericAttr.create("focalLength", "focalLength", MFnNumericData::kDouble);
    numericAttr.setWritable(false);
    numericAttr.setStorable(false);
    addAttribute(aOutputFocalLength);

    aOutputAperture = numericAttr.create("outputAperture", "outputAperture", MFnNumericData::kDouble);
    numericAttr.setWritable(false);
    numericAttr.setStorable(false);
    addAttribute(aOutputAperture);

    //matrix Attrbituies

    MFnMatrixAttribute matrixAttr;

    aCameraWorldMatrix = matrixAttr.create("cameraWorldMatrix", "cameraWorldMatrix", MFnMatrixAttribute::kDouble);
    matrixAttr.setKeyable(true);
    addAttribute(aCameraWorldMatrix);

    aTargetWorldMatrix = matrixAttr.create("targetWorldMatrix", "targetWorldMatrix", MFnMatrixAttribute::kDouble);
    matrixAttr.setKeyable(true);
    addAttribute(aTargetWorldMatrix);

    //dirty propapgation for output focal length

    attributeAffects(aCameraHorizontalAperture, aOutputFocalLength);
    attributeAffects(aCameraWorldMatrix, aOutputFocalLength);
    attributeAffects(aTargetWorldMatrix, aOutputFocalLength);
    attributeAffects(aWidth, aOutputFocalLength);

    //dirty propapgation for output distance
    attributeAffects(aCameraHorizontalAperture, aDistance);
    attributeAffects(aCameraWorldMatrix, aDistance);
    attributeAffects(aTargetWorldMatrix, aDistance);
    attributeAffects(aWidth, aDistance);


    attributeAffects(aCameraHorizontalAperture, aOutputAperture);
    return status;
}


MStatus DollyZoomNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status(MS::kSuccess);
    if (plug != aOutputFocalLength && plug != aDistance && plug != aOutputAperture)
    {
        return MS::kUnknownParameter;
    }
    if (plug == aOutputAperture)
    {
    
        double aperture = data.inputValue(aCameraHorizontalAperture, &status).asDouble();

        aperture = DollyZoom::milimetetersToinches(aperture);

        MDataHandle hAperture = data.outputValue(aOutputAperture, &status);
        hAperture.setDouble(aperture);
        hAperture.setClean();

        data.setClean(plug);
        
        return status;
    
    }
    double width = data.inputValue(aWidth, &status).asDouble();

    if (width <= 0.0)
    {
        width = 0.001;
    }

    double cameraAperture = data.inputValue(aCameraHorizontalAperture, &status).asDouble();

    if (cameraAperture<= 0.0)
    {
        cameraAperture= 0.001;
    }
    
    // get the world matrix of our camera and target. it initializes a MTransformationMatrix for accesing the positions

    MTransformationMatrix cameraWorldMatrix = data.inputValue(aCameraWorldMatrix, &status).asMatrix();
    MTransformationMatrix targetWorldMatrix = data.inputValue(aTargetWorldMatrix, &status).asMatrix();


    //Extracting the  world space position from our matrix and storing it in MPoints to calculate their distance
    MPoint cameraTranslate(cameraWorldMatrix.getTranslation(MSpace::kWorld));
    MPoint targetTranslate(targetWorldMatrix.getTranslation(MSpace::kWorld));

    //we will use this distance later to output as a plug
    double distance = cameraTranslate.distanceTo(targetTranslate);
    MStreamUtils::stdOutStream() << distance<< '\n';
    if (distance <= 0.0)
    {
        distance = 0.001;
    }
    //Calculate new AOV
    double newFOV{ DollyZoom::calculateAOV(width, distance) }; //returns new AOV, we need to convert to Focal Length
    
    newFOV = DollyZoom::angleOfViewToFocalLength(newFOV, cameraAperture);

    //Data handles for output attributes (focal length and distance)
    MDataHandle hOutputFocalLength = data.outputValue(aOutputFocalLength, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    MDataHandle hOutputDistace = data.outputValue(aDistance, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);


    //calculations are completed, set the plugs and data clean
    hOutputFocalLength.setDouble(newFOV);
    hOutputFocalLength.setClean();

    hOutputDistace.setDouble(distance);
    hOutputDistace.setClean();
   

    data.setClean(plug);


    return status;

}


