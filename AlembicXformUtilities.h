#ifndef __ALEMBIC_TRANSFORM_UTILITIES_H
#define __ALEMBIC_TRANSFORM_UTILITIES_H

#include "Alembic.h"
#include "AlembicArchiveStorage.h"
#include "AlembicDefinitions.h"
#include "AlembicXformController.h"
#include "Utility.h"
#include "dummy.h"
#include <ILockedTracks.h>
#include "iparamb2.h"
#include "AlembicNames.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// alembic_fillxform_options
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _alembic_fillxform_options
{
    Alembic::AbcGeom::IObject *pIObj;
    Matrix3 maxMatrix;
    Box3    maxBoundingBox;
    TimeValue dTicks;
    bool bIsCameraTransform;

    _alembic_fillxform_options()
    {
        pIObj = 0;
        dTicks = 0;
        maxMatrix.IdentityMatrix();
        bIsCameraTransform = false;
    }
} alembic_fillxform_options;


void AlembicImport_FillInXForm(alembic_fillxform_options &options);
int AlembicImport_XForm(INode* pMaxNode, Alembic::AbcGeom::IObject& iObj, const std::string &file, alembic_importoptions &options);
int AlembicImport_DummyNode(Alembic::AbcGeom::IObject& iObj, alembic_importoptions &options, INode** pMaxNode);

size_t getNumXformChildren( Alembic::AbcGeom::IObject& iObj );

#endif	// __ALEMBIC_TRANSFORM_UTILITIES_H