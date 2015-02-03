
#include "ms3d.h"
#include "../texture.h"
#include "../utils.h"
#include "model.h"
#include "../platform.h"
#include "../math/vec3f.h"
#include "../math/quaternion.h"
#include "../math/matrix.h"
#include "../math/3dmath.h"
#include "../math/matrix.h"
#include "vertexarray.h"
#include "../save/saveedm.h"

//int m_frame = 0;

MS3DModel::MS3DModel()
{
	m_numMeshes = 0;
	m_pMeshes = NULL;
	m_numMaterials = 0;
	m_pMaterials = NULL;
	m_numTriangles = 0;
	m_pTriangles = NULL;
	m_numVertices = 0;
	m_pVertices = NULL;
	m_numJoints = 0;
	m_pJoints = NULL;
}

MS3DModel::~MS3DModel()
{
	int i;
	for ( i = 0; i < m_numMeshes; i++ )
		delete[] m_pMeshes[i].m_pTriangleIndices;
	for ( i = 0; i < m_numMaterials; i++ )
		delete[] m_pMaterials[i].m_pTextureFilename;

	m_numMeshes = 0;
	if ( m_pMeshes != NULL )
	{
		delete[] m_pMeshes;
		m_pMeshes = NULL;
	}

	m_numMaterials = 0;
	if ( m_pMaterials != NULL )
	{
		delete[] m_pMaterials;
		m_pMaterials = NULL;
	}

	m_numTriangles = 0;
	if ( m_pTriangles != NULL )
	{
		delete[] m_pTriangles;
		m_pTriangles = NULL;
	}

	m_numVertices = 0;
	if ( m_pVertices != NULL )
	{
		delete[] m_pVertices;
		m_pVertices = NULL;
	}

	m_numJoints = 0;
	if(m_pJoints != NULL)
	{
		delete [] m_pJoints;
		m_pJoints = NULL;
	}
}

void MS3DModel::loadtex(unsigned int& diffm, unsigned int& specm, unsigned int& normm)
{
	for ( int i = 0; i < m_numMaterials; i++ )
		if ( strlen( m_pMaterials[i].m_pTextureFilename ) > 0 )
		{
			//QueueTexture(&m_pMaterials[i].m_diffusem, m_pMaterials[i].m_pTextureFilename, true);

			char basefile[MAX_PATH+1];
			strcpy(basefile, m_pMaterials[i].m_pTextureFilename);
			StripPath(basefile);
			char basename[MAX_PATH+1];
			sprintf(basename, "%s%s", m_relative, basefile);
			char difffile[MAX_PATH+1];
			strcpy(difffile, basename);
			StripExtension(basename);
			char specfile[MAX_PATH+1];
			char normfile[MAX_PATH+1];
			SpecPath(basename, specfile);
			NormPath(basename, normfile);

			QueueTexture(&diffm, difffile, false);
			QueueTexture(&specm, specfile, false);
			QueueTexture(&normm, normfile, false);
		}
		//else
		//	m_pMaterials[i].m_diffusem = 0;
}

bool MS3DModel::load(const char *relative, unsigned int& diffm, unsigned int& specm, unsigned int& normm)
{
	char full[MAX_PATH+1];
	FullPath(relative, full);

	ifstream inputFile( full, ios::in | ios::binary );
	if ( inputFile.fail()) 
	{
		g_log << "Couldn't open the model file "<< relative << endl;
		return false;
	}

	string reltemp = StripFile(relative);

	//if(strlen(reltemp.c_str()) == 0)
	//	reltemp += CORRECT_SLASH;

	strcpy(m_relative, reltemp.c_str());

	/*
	char pathTemp[MAX_PATH+1];
	int pathLength;
	for ( pathLength = strlen( filename ); --pathLength; ) 
	{
		if ( filename[pathLength] == '/' || filename[pathLength] == '\\' ) 
			break;
	}
	strncpy( pathTemp, filename, pathLength );
	
	int i;
	if ( pathLength > 0 ) 
	{
		pathTemp[pathLength++] = '/';
	}
	
	strncpy( m_filepath, filename, pathLength );
	*/

	inputFile.seekg( 0, ios::end );
	long fileSize = inputFile.tellg();
	inputFile.seekg( 0, ios::beg );

	char *pBuffer = new char[fileSize];
	inputFile.read( pBuffer, fileSize );
	inputFile.close();

	const char *pPtr = pBuffer;
	MS3DHeader *pHeader = ( MS3DHeader* )pPtr;
	pPtr += sizeof( MS3DHeader );

	if ( strncmp( pHeader->m_ID, "MS3D000000", 10 ) != 0 ) 
	{
		g_log << "Not an MS3D file "<< relative << endl;
		return false;
    }

	if ( pHeader->m_version < 3 ) 
	{
		g_log << "I know nothing about MS3D v1.2, " <<relative<< endl;
		return false;
	}

	int nVertices = *( word* )pPtr; 
	m_numVertices = nVertices;
	m_pVertices = new Vertex[nVertices];
	pPtr += sizeof( word );

	int i;
	for ( i = 0; i < nVertices; i++ )
	{
		MS3DVertex *pVertex = ( MS3DVertex* )pPtr;
		m_pVertices[i].m_boneID = pVertex->m_boneID;
		memcpy( m_pVertices[i].m_location, pVertex->m_vertex, sizeof( float )*3 );
		pPtr += sizeof( MS3DVertex );
	}

	int nTriangles = *( word* )pPtr;
	m_numTriangles = nTriangles;
	m_pTriangles = new Triangle[nTriangles];
	pPtr += sizeof( word );

	for ( i = 0; i < nTriangles; i++ )
	{
		MS3DTriangle *pTriangle = ( MS3DTriangle* )pPtr;
		int vertexIndices[3] = { pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2] };
		float t[3] = { 1.0f-pTriangle->m_t[0], 1.0f-pTriangle->m_t[1], 1.0f-pTriangle->m_t[2] };
		memcpy( m_pTriangles[i].m_vertexNormals, pTriangle->m_vertexNormals, sizeof( float )*3*3 );
		memcpy( m_pTriangles[i].m_s, pTriangle->m_s, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_t, t, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_vertexIndices, vertexIndices, sizeof( int )*3 );
		pPtr += sizeof( MS3DTriangle );
	}

	int nGroups = *( word* )pPtr;
	m_numMeshes = nGroups;
	m_pMeshes = new Mesh[nGroups];
	pPtr += sizeof( word );
	for ( i = 0; i < nGroups; i++ )
	{
		pPtr += sizeof( byte );	// flags
		const char* groupname = pPtr;
		pPtr += 32;				// name

		word nTriangles = *( word* )pPtr;
		pPtr += sizeof( word );
		int *pTriangleIndices = new int[nTriangles];
		for ( int j = 0; j < nTriangles; j++ )
		{
			pTriangleIndices[j] = *( word* )pPtr;
			pPtr += sizeof( word );
		}

		char materialIndex = *( char* )pPtr;
		pPtr += sizeof( char );

		m_pMeshes[i].m_materialIndex = materialIndex;
		m_pMeshes[i].m_numTriangles = nTriangles;
		m_pMeshes[i].m_pTriangleIndices = pTriangleIndices;
	}

	int nMaterials = *( word* )pPtr;
	m_numMaterials = nMaterials;
	m_pMaterials = new Material[nMaterials];
	pPtr += sizeof( word );
	for ( i = 0; i < nMaterials; i++ )
	{
		MS3DMaterial *pMaterial = ( MS3DMaterial* )pPtr;
		memcpy( m_pMaterials[i].m_ambient, pMaterial->m_ambient, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_diffuse, pMaterial->m_diffuse, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_specular, pMaterial->m_specular, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_emissive, pMaterial->m_emissive, sizeof( float )*4 );
		m_pMaterials[i].m_shininess = pMaterial->m_shininess;
		if ( strncmp( pMaterial->m_diffusem, ".\\", 2 ) == 0 ) {
			// MS3D 1.5.x relative path
			//StripPath(pMaterial->m_diffusem);
			//m_pMaterials[i].m_pTextureFilename = new char[ strlen(relativepath.c_str()) + strlen(pMaterial->m_diffusem) + 1 ];
			//strcpy( m_pMaterials[i].m_pTextureFilename, reltemp.c_str() );
			//sprintf(m_pMaterials[i].m_pTextureFilename, "%s%s", relativepath.c_str(), pMaterial->m_diffusem);
			m_pMaterials[i].m_pTextureFilename = new char[strlen( pMaterial->m_diffusem )+1];
			strcpy( m_pMaterials[i].m_pTextureFilename, pMaterial->m_diffusem );
		}
		else {
			// MS3D 1.4.x or earlier - absolute path
			m_pMaterials[i].m_pTextureFilename = new char[strlen( pMaterial->m_diffusem )+1];
			strcpy( m_pMaterials[i].m_pTextureFilename, pMaterial->m_diffusem );
		}
		StripPath(m_pMaterials[i].m_pTextureFilename);
		pPtr += sizeof( MS3DMaterial );
	}

	loadtex(diffm, specm, normm);

	float animFPS = *( float* )pPtr;
	pPtr += sizeof( float );

	// skip currentTime
	pPtr += sizeof( float );

	m_totalFrames = *( int* )pPtr;
	pPtr += sizeof( int );

	m_totalTime = m_totalFrames*1000.0/animFPS;

	m_numJoints = *( word* )pPtr;
	pPtr += sizeof( word );

	m_pJoints = new Joint[m_numJoints];

	struct JointNameListRec
	{
		int m_jointIndex;
		const char *m_pName;
	};

	const char *pTempPtr = pPtr;

	JointNameListRec *pNameList = new JointNameListRec[m_numJoints];
	for ( i = 0; i < m_numJoints; i++ )
	{
		MS3DJoint *pJoint = ( MS3DJoint* )pTempPtr;
		pTempPtr += sizeof( MS3DJoint );
		pTempPtr += sizeof( MS3DKeyframe )*( pJoint->m_numRotationKeyframes+pJoint->m_numTranslationKeyframes );

		pNameList[i].m_jointIndex = i;
		pNameList[i].m_pName = pJoint->m_name;
	}

	for ( i = 0; i < m_numJoints; i++ )
	{
		MS3DJoint *pJoint = ( MS3DJoint* )pPtr;
		pPtr += sizeof( MS3DJoint );

		int j, parentIndex = -1;
		if ( strlen( pJoint->m_parentName ) > 0 )
		{
			for ( j = 0; j < m_numJoints; j++ )
			{
				if ( _stricmp( pNameList[j].m_pName, pJoint->m_parentName ) == 0 )
				{
					parentIndex = pNameList[j].m_jointIndex;
					break;
				}
			}
			if ( parentIndex == -1 ) {
				g_log << "Unable to find parent bone in MS3D file" << endl;
				return false;
			}
		}

		memcpy( m_pJoints[i].m_localRotation, pJoint->m_rotation, sizeof( float )*3 );
		memcpy( m_pJoints[i].m_localTranslation, pJoint->m_translation, sizeof( float )*3 );
		m_pJoints[i].m_parent = parentIndex;
		m_pJoints[i].m_numRotationKeyframes = pJoint->m_numRotationKeyframes;
		m_pJoints[i].m_pRotationKeyframes = new Keyframe[pJoint->m_numRotationKeyframes];
		m_pJoints[i].m_numTranslationKeyframes = pJoint->m_numTranslationKeyframes;
		m_pJoints[i].m_pTranslationKeyframes = new Keyframe[pJoint->m_numTranslationKeyframes];

		for ( j = 0; j < pJoint->m_numRotationKeyframes; j++ )
		{
			MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
			pPtr += sizeof( MS3DKeyframe );

			setjointkf( i, j, pKeyframe->m_time*1000.0f, pKeyframe->m_parameter, true );
		}

		for ( j = 0; j < pJoint->m_numTranslationKeyframes; j++ )
		{
			MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
			pPtr += sizeof( MS3DKeyframe );

			setjointkf( i, j, pKeyframe->m_time*1000.0f, pKeyframe->m_parameter, false );
		}
	}
	delete[] pNameList;

	setupjoints();

	delete[] pBuffer;

	restart();

	g_log<<relative<<"\n\r";

	return true;
}

void MS3DModel::genva(VertexArray** vertexArrays, Vec3f scale, Vec3f translate, const char* filepath)
{
	(*vertexArrays) = new VertexArray[ m_totalFrames ];

	Vec3f* vertices;
	Vec2f* texcoords;
	Vec3f* normals;
	int* orignums;

	int numverts = 0;

	for ( int i = 0; i < m_numMeshes; i++ )
		numverts += m_pMeshes[i].m_numTriangles*3;

	for(int n = 0; n < m_totalFrames; n++)
	{
		//(*vertexArrays)[n].numverts = numverts;
		//(*vertexArrays)[n].vertices = new Vec3f[ numverts ];
		//(*vertexArrays)[n].texcoords = new Vec2f[ numverts ];
		//(*vertexArrays)[n].normals = new Vec3f[ numverts ];
		(*vertexArrays)[n].alloc(numverts);
	}

	restart();

	vector<Vec3f>* normalweights;

	normalweights = new vector<Vec3f>[numverts];

	for(int f = 0; f < m_totalFrames; f++)
	{
		advanceanim();

#if 0
		g_log<<endl<<endl;
		g_log<<"frame #"<<f<<endl;
#endif

		int vert = 0;

		vertices = (*vertexArrays)[f].vertices;
		texcoords = (*vertexArrays)[f].texcoords;
		normals = (*vertexArrays)[f].normals;
		orignums = (*vertexArrays)[f].orignum;

		for(int index = 0; index < m_numVertices; index++)
		{
			normalweights[index].clear();
		}
  
		for(int i = 0; i < m_numMeshes; i++)
		{
			for(int j = 0; j < m_pMeshes[i].m_numTriangles; j++)
			{
				int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
				const Triangle* pTri = &m_pTriangles[triangleIndex];

				for(int k = 0; k < 3; k++)
				{
					int index = pTri->m_vertexIndices[k];

					//int orignum = j*3 + k;
					orignums[vert] = index;

#if 0
					if(index == 80)
					{
						g_log<<"80parentbone = "<<(int)m_pVertices[index].m_boneID<<endl;
					}
#endif

					if(m_pVertices[index].m_boneID == -1)
					{
						//g_log<<"\tno tran"<<endl;
						texcoords[vert].x = pTri->m_s[k];
						texcoords[vert].y = 1.0f - pTri->m_t[k];

						normals[vert].x = pTri->m_vertexNormals[k][0];
						normals[vert].y = pTri->m_vertexNormals[k][1];
						normals[vert].z = pTri->m_vertexNormals[k][2];

						vertices[vert].x = m_pVertices[index].m_location[0] * scale.x + translate.x;
						vertices[vert].y = m_pVertices[index].m_location[1] * scale.y + translate.y;
						vertices[vert].z = m_pVertices[index].m_location[2] * scale.z + translate.z;
					}
					else
					{
						// rotate according to transformation matrix
						const Matrix& final = m_pJoints[m_pVertices[index].m_boneID].m_final;

						texcoords[vert].x = pTri->m_s[k];
						texcoords[vert].y = 1.0f - pTri->m_t[k];

						Vec3f newNormal(pTri->m_vertexNormals[k]);
						newNormal.transform3(final);
						newNormal = Normalize(newNormal);

						normals[vert] = newNormal;

						Vec3f newVertex(m_pVertices[index].m_location);
						newVertex.transform(final);
						
#if 0
						if(vert == 384)
						{
							for(int miter = 0; miter < 16; miter+=4)
								g_log<<"384matrix"<<miter<<": "<<final.m_matrix[miter+0]<<","<<final.m_matrix[miter+1]<<","<<final.m_matrix[miter+2]<<","<<final.m_matrix[miter+3]<<","<<endl;

							g_log<<"384before: "<<m_pVertices[index].m_location[0]<<","<<m_pVertices[index].m_location[1]<<","<<m_pVertices[index].m_location[2]<<endl;
							g_log<<"384after: "<<newVertex.x<<","<<newVertex.y<<","<<newVertex.z<<endl;
						}
#endif
						
#if 1
						//if(vert != 384)
						if(1)
						{
						vertices[vert].x = newVertex.x * scale.x + translate.x;
						vertices[vert].y = newVertex.y * scale.y + translate.y;
						vertices[vert].z = newVertex.z * scale.z + translate.z;
						}
						else
						{
							
						vertices[vert].x = m_pVertices[index].m_location[0] * scale.x + translate.x;
						vertices[vert].y = m_pVertices[index].m_location[1] * scale.y + translate.y;
						vertices[vert].z = m_pVertices[index].m_location[2] * scale.z + translate.z;
						}
#else					
						vertices[vert].x = m_pVertices[index].m_location[0] * scale.x + translate.x;
						vertices[vert].y = m_pVertices[index].m_location[1] * scale.y + translate.y;
						vertices[vert].z = m_pVertices[index].m_location[2] * scale.z + translate.z;
#endif

#if 0
						Vec3f off;
						off.set(m_pVertices[index].m_location);
						off = off * scale + translate;
						off = vertices[vert] - off;
						
						g_log<<"\tyes tran "<<off.x<<","<<off.y<<","<<off.z<<" "<<endl;
#endif
					}

					//vert ++;

					// Reverse vertex order
					//0=>2=>1

					if(vert % 3 == 0)
						vert += 2;
					else if(vert % 3 == 2)
						vert --;
					else if(vert % 3 == 1)
						vert += 2;

				}

				Vec3f normal;
				Vec3f tri[3];
				tri[0] = vertices[vert-3];
				tri[1] = vertices[vert-3+1];
				tri[2] = vertices[vert-3+2];
				//normal = Normal2(tri);
				normal = Normal(tri);	//Reverse order
				//normals[i] = normal;
				//normals[i+1] = normal;
				//normals[i+2] = normal;

				for(int k = 0; k < 3; k++)
				{
					int index = pTri->m_vertexIndices[k];
					normalweights[index].push_back(normal);
				}
			}
		}

#if 0
		vert = 0;

		for(int i = 0; i < m_numMeshes; i++)
		{
			for(int j = 0; j < m_pMeshes[i].m_numTriangles; j++)
			{
				int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
				const Triangle* pTri = &m_pTriangles[triangleIndex];

				for(int k = 0; k < 3; k++)
				{
					int index = pTri->m_vertexIndices[k];

					Vec3f weighsum(0, 0, 0);

					for(int l=0; l<normalweights[index].size(); l++)
					{/*
						if(strstr(filepath, "flat"))
						{
							//g_log<<"weighsum + "<<normalweights[index][l].x<<","<<normalweights[index][l].y<<","<<normalweights[index][l].z<<endl;
							//g_log.flush();
						}*/

						weighsum = weighsum + normalweights[index][l] / (float)normalweights[index].size();
					}
					/*
					if(strstr(filepath, "flat"))
					{
						g_log<<"weighsum = "<<weighsum.x<<","<<weighsum.y<<","<<weighsum.z<<endl;
						g_log.flush();
					}*/

					normals[vert] = Normalize(weighsum);
					//normals[vert+1] = weighsum;
					//normals[vert+2] = weighsum;

					//vert ++;

					// Reverse vertex order
					//0=>2=>1

					if(vert % 3 == 0)
						vert += 2;
					else if(vert % 3 == 2)
						vert --;
					else if(vert % 3 == 1)
						vert += 2;
				}
			}
		}
#endif
	}

	delete [] normalweights;
}

void MS3DModel::setjointkf( int jointIndex, int keyframeIndex, float time, float *parameter, bool isRotation )
{
	Keyframe& keyframe = isRotation ? m_pJoints[jointIndex].m_pRotationKeyframes[keyframeIndex] :
		m_pJoints[jointIndex].m_pTranslationKeyframes[keyframeIndex];

	keyframe.m_jointIndex = jointIndex;
	keyframe.m_time = time;
	memcpy( keyframe.m_parameter, parameter, sizeof( float )*3 );
}

void MS3DModel::setupjoints()
{
	int i;
	for ( i = 0; i < m_numJoints; i++ )
	{
		Joint& joint = m_pJoints[i];

		joint.m_relative.setRotationRadians( joint.m_localRotation );
		joint.m_relative.setTranslation( joint.m_localTranslation );
		if ( joint.m_parent != -1 )
		{
			joint.m_absolute.set( m_pJoints[joint.m_parent].m_absolute.m_matrix);
			joint.m_absolute.postMultiply( joint.m_relative );
		}
		else
			joint.m_absolute.set( joint.m_relative.m_matrix);
	}

	for ( i = 0; i < m_numVertices; i++ )
	{
		Vertex& vertex = m_pVertices[i];

		if ( vertex.m_boneID != -1 )
		{
			Matrix& matrix = m_pJoints[vertex.m_boneID].m_absolute;

			matrix.inverseTranslateVect( vertex.m_location );
			matrix.inverseRotateVect( vertex.m_location );
		}
	}

	for ( i = 0; i < m_numTriangles; i++ ) {
		Triangle& triangle = m_pTriangles[i];
		for ( int j = 0; j < 3; j++ ) {
			const Vertex& vertex = m_pVertices[triangle.m_vertexIndices[j]];
			if ( vertex.m_boneID != -1 ) {
				Matrix& matrix = m_pJoints[vertex.m_boneID].m_absolute;
				matrix.inverseRotateVect( triangle.m_vertexNormals[j] );
			}
		}
	}
}

void MS3DModel::restart()
{
	m_frame = 0;
	for ( int i = 0; i < m_numJoints; i++ )
	{
		m_pJoints[i].m_currentRotationKeyframe = m_pJoints[i].m_currentTranslationKeyframe = 0;
		m_pJoints[i].m_final.set( m_pJoints[i].m_absolute.m_matrix);
	}
}

void MS3DModel::advanceanim()
{
	//static int frame = 0;

	double time = m_totalTime * (double)m_frame/(double)m_totalFrames;

	m_frame++;

	if(time > m_totalTime)
	{
		restart();
		time = 0;
	}

	for(int i = 0; i < m_numJoints; i++)
	{
		float transVec[3];
		Matrix transform;
		int frame;
		Joint *pJoint = &m_pJoints[i];

		if(pJoint->m_numRotationKeyframes == 0 && pJoint->m_numTranslationKeyframes == 0)
		{
#if 0
			pJoint->m_final.set( pJoint->m_absolute.m_matrix);
#else
			Matrix relativeFinal( pJoint->m_relative );

			if ( pJoint->m_parent == -1 )
				pJoint->m_final.set( relativeFinal.m_matrix );
			else
			{
				pJoint->m_final.set( m_pJoints[pJoint->m_parent].m_final.m_matrix );
				pJoint->m_final.postMultiply( relativeFinal );
			}
#endif

			continue;
		}

		frame = pJoint->m_currentTranslationKeyframe;
		while(frame < pJoint->m_numTranslationKeyframes && pJoint->m_pTranslationKeyframes[frame].m_time < time)
		{
			frame++;
		}
		pJoint->m_currentTranslationKeyframe = frame;
		
		//g_log<<"tr kff "<<frame<<" time "<<pJoint->m_pTranslationKeyframes[frame].m_time<<","<<time<<endl;

		if(frame == 0)
			memcpy( transVec, pJoint->m_pTranslationKeyframes[0].m_parameter, sizeof ( float )*3 );
		else if(frame == pJoint->m_numTranslationKeyframes)
			memcpy( transVec, pJoint->m_pTranslationKeyframes[frame-1].m_parameter, sizeof ( float )*3 );
		else
		{
			//assert( frame > 0 && frame < pJoint->m_numTranslationKeyframes );

			const MS3DModel::Keyframe& curFrame = pJoint->m_pTranslationKeyframes[frame];
			const MS3DModel::Keyframe& prevFrame = pJoint->m_pTranslationKeyframes[frame-1];

			float timeDelta = curFrame.m_time-prevFrame.m_time;
			float interpValue = ( float )(( time-prevFrame.m_time )/timeDelta );

			transVec[0] = prevFrame.m_parameter[0]+( curFrame.m_parameter[0]-prevFrame.m_parameter[0] )*interpValue;
			transVec[1] = prevFrame.m_parameter[1]+( curFrame.m_parameter[1]-prevFrame.m_parameter[1] )*interpValue;
			transVec[2] = prevFrame.m_parameter[2]+( curFrame.m_parameter[2]-prevFrame.m_parameter[2] )*interpValue; 
		}

		frame = pJoint->m_currentRotationKeyframe;
		while(frame < pJoint->m_numRotationKeyframes && pJoint->m_pRotationKeyframes[frame].m_time < time)
		{
			frame++;
		}
		pJoint->m_currentRotationKeyframe = frame;

		//g_log<<"rot kff "<<frame<<" time "<<pJoint->m_pRotationKeyframes[frame].m_time<<","<<time<<endl;

		if(frame == 0)
		{
			transform.setRotationRadians( pJoint->m_pRotationKeyframes[0].m_parameter );
			
			//g_log<<"rotvec0 "<<pJoint->m_pRotationKeyframes[0].m_parameter[0]<<","<<pJoint->m_pRotationKeyframes[0].m_parameter[1]<<","<<pJoint->m_pRotationKeyframes[0].m_parameter[2]<<endl;
		}
		else if(frame == pJoint->m_numRotationKeyframes)
		{
			transform.setRotationRadians( pJoint->m_pRotationKeyframes[frame-1].m_parameter );
			
			//g_log<<"rotvecn-1 "<<pJoint->m_pRotationKeyframes[frame-1].m_parameter[0]<<","<<pJoint->m_pRotationKeyframes[frame-1].m_parameter[1]<<","<<pJoint->m_pRotationKeyframes[frame-1].m_parameter[2]<<endl;
		}
		else
		{
			const MS3DModel::Keyframe& curFrame = pJoint->m_pRotationKeyframes[frame];
			const MS3DModel::Keyframe& prevFrame = pJoint->m_pRotationKeyframes[frame-1];

			float timeDelta = curFrame.m_time-prevFrame.m_time;
			float interpValue = (float)(( time-prevFrame.m_time )/timeDelta );

#if 0
			Quaternion qPrev( prevFrame.m_parameter );
			Quaternion qCur( curFrame.m_parameter );
			Quaternion qFinal( qPrev, qCur, interpValue );
			transform.setRotationQuaternion( qFinal );
#else
			float rotVec[3];

			rotVec[0] = prevFrame.m_parameter[0]+( curFrame.m_parameter[0]-prevFrame.m_parameter[0] )*interpValue;
			rotVec[1] = prevFrame.m_parameter[1]+( curFrame.m_parameter[1]-prevFrame.m_parameter[1] )*interpValue;
			rotVec[2] = prevFrame.m_parameter[2]+( curFrame.m_parameter[2]-prevFrame.m_parameter[2] )*interpValue;

			transform.setRotationRadians( rotVec );

			//g_log<<"rotvec "<<rotVec[0]<<","<<rotVec[1]<<","<<rotVec[2]<<endl;
#endif
		}

		transform.setTranslation( transVec );
		Matrix relativeFinal( pJoint->m_relative );
		//relativeFinal.loadIdentity();
		relativeFinal.postMultiply( transform );

		if ( pJoint->m_parent == -1 )
			pJoint->m_final.set( relativeFinal.m_matrix );
		else
		{
			pJoint->m_final.set( m_pJoints[pJoint->m_parent].m_final.m_matrix );

			//if(i != 27 && i != 39 && i!= 26)
			pJoint->m_final.postMultiply( relativeFinal );
		}
	}
	
#if 0
	for(int i = 0; i < m_numJoints; i++)
	{
		float transVec[3];
		Matrix transform;
		int frame;
		Joint *pJoint = &m_pJoints[i];

		float* m = pJoint->m_final.m_matrix;
		
		g_log<<endl<<endl;
		g_log<<"joint #"<<i<<"  of "<<pJoint->m_parent<<endl;
		g_log<<"["<<m[0]<<","<<m[1]<<","<<m[2]<<","<<m[3]<<"]"<<endl;
		g_log<<"["<<m[4]<<","<<m[5]<<","<<m[6]<<","<<m[7]<<"]"<<endl;
		g_log<<"["<<m[8]<<","<<m[9]<<","<<m[10]<<","<<m[11]<<"]"<<endl;
		g_log<<"["<<m[12]<<","<<m[13]<<","<<m[14]<<","<<m[15]<<"]"<<endl;
		g_log<<endl<<endl;
	}
#endif
}