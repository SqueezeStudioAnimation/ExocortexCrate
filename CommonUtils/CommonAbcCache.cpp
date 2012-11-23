#include "CommonAlembic.h"
#include "CommonMeshUtilities.h"
#include "CommonUtilities.h"
#include "CommonAbcCache.h"

AbcObjectCache::AbcObjectCache( Alembic::Abc::IObject & objToCache )
		:	obj( objToCache ), isMeshTopoDynamic(false), isMeshPointCache(false),
			fullName(objToCache.getFullName()), numSamples(getNumSamplesFromObject(objToCache)), isConstant(isObjectConstant(objToCache))
{
	ESS_PROFILE_SCOPE("AbcObjectCache::AbcObjectCache");
	//EC_LOG_WARNING( "fullname: " << objToCache.getFullName() );
	const AbcA::MetaData &mData = objToCache.getMetaData();
	if( AbcG::IPolyMesh::matches(mData) || AbcG::ISubD::matches(mData)) {
		if( ! isConstant ) {
			isMeshTopoDynamic = isAlembicMeshTopoDynamic( &objToCache );
		}
		isMeshPointCache = isAlembicMeshPointCache( &objToCache );
	}
}

AbcObjectCache* addObjectToCache( AbcArchiveCache* fullNameToObjectCache, Abc::IObject &obj, std::string parentIdentifier ) {
	ESS_PROFILE_SCOPE("addObjectToCache");
	AbcObjectCache objectCache( obj );
	objectCache.parentIdentifier = parentIdentifier;
	for( int i = 0; i < obj.getNumChildren(); i ++ ) {
		Abc::IObject child = obj.getChild( i );
		AbcObjectCache* childObjectCache = addObjectToCache( fullNameToObjectCache, child, objectCache.fullName );
		objectCache.childIdentifiers.push_back( childObjectCache->fullName );
	}
	fullNameToObjectCache->insert( AbcArchiveCache::value_type( objectCache.fullName, objectCache ) );
	return &( fullNameToObjectCache->find( objectCache.fullName )->second );
}

void createAbcArchiveCache( Abc::IArchive *pArchive, AbcArchiveCache* fullNameToObjectCache ) {
	ESS_PROFILE_SCOPE("createAbcArchiveCache");
	EC_LOG_INFO( "Creating AbcArchiveCache for archive: " << pArchive->getName() );
   Abc::IObject top = pArchive->getTop();
	addObjectToCache( fullNameToObjectCache, top, "" );
}
