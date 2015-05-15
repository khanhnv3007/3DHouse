namespace ExternalModels {

	struct Mesh {
		unsigned int * faceList;
		GLfloat * vertexArray;
		GLfloat * texCoordArray;
		GLfloat * normalArray;
	};

	class ExternalModel {
	private:
		const aiScene* scene;
		GLfloat scale;
		GLfloat internalX, internalY, internalZ;
		GLfloat x, y, z;
		GLuint * textureArray;
		GLint numTexture;
		aiString * nameToTex;
		aiVector3D min, max;
		GLfloat angle;
		
		Mesh rootMesh;
		Mesh * meshList;
		int numMesh;
		
		bool customTexture;
		
		void color4_to_float4 (const aiColor4D *c, float f[4])
		{
			f[0] = c->r;
			f[1] = c->g;
			f[2] = c->b;
			f[3] = c->a;
		}
		
		// ----------------------------------------------------------------------------
		// From AssImp library sample
		// 
		
		#define aisgl_min(x,y) (x<y?x:y)
		#define aisgl_max(x,y) (y>x?y:x)
		
		void get_bounding_box_for_node (const aiNode* nd, aiVector3D* min, aiVector3D* max, aiMatrix4x4* trafo) {
			aiMatrix4x4 prev;
			unsigned int n = 0, t;

			prev = *trafo;
			aiMultiplyMatrix4(trafo,&nd->mTransformation);

			for (; n < nd->mNumMeshes; ++n) {
				const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
				for (t = 0; t < mesh->mNumVertices; ++t) {

					aiVector3D tmp = mesh->mVertices[t];
					aiTransformVecByMatrix4(&tmp,trafo);

					min->x = aisgl_min(min->x,tmp.x);
					min->y = aisgl_min(min->y,tmp.y);
					min->z = aisgl_min(min->z,tmp.z);

					max->x = aisgl_max(max->x,tmp.x);
					max->y = aisgl_max(max->y,tmp.y);
					max->z = aisgl_max(max->z,tmp.z);
				}
			}

			for (n = 0; n < nd->mNumChildren; ++n) {
				get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
			}
			*trafo = prev;
		}

		void get_bounding_box (aiVector3D* min, aiVector3D* max)
		{
			aiMatrix4x4 trafo;
			aiIdentityMatrix4(&trafo);

			min->x = min->y = min->z =  1e10f;
			max->x = max->y = max->z = -1e10f;
			get_bounding_box_for_node (scene->mRootNode,min,max,&trafo);
		}
		
		#undef aisgl_min
		#undef aisgl_max
		
		// 
		// ----------------------------------------------------------------------------
		
		GLuint findTexture (aiString & path) {
			for (int i = 0; i < numTexture; i ++) {
				if (nameToTex[i] == path) {
					return textureArray[i];
				}
			}
		}
		
		void apply_material (const aiMaterial* mtl)
		{
			float c[4];

			GLenum fill_mode;
			int ret1, ret2;
			aiColor4D diffuse, specular, ambient, emission;
			float shininess, strength;
			int two_sided;
			int wireframe;
			unsigned int max;
			
			aiString texPath;
			if (AI_SUCCESS == aiGetMaterialTexture (mtl, aiTextureType_DIFFUSE, 0, &texPath))
			{
				glBindTexture (GL_TEXTURE_2D, findTexture (texPath));
			}
			else {
				/* */
			}

			setArray4f (c, 0.8f, 0.8f, 0.8f, 1.0f); // Default value for diffuse
			if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
				color4_to_float4 (&diffuse, c);
			glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, c);

			setArray4f (c, 0.0f, 0.0f, 0.0f, 1.0f); // Default value for specular
			if (AI_SUCCESS == aiGetMaterialColor (mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
				color4_to_float4 (&specular, c);
			glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, c);

			setArray4f (c, 0.2f, 0.2f, 0.2f, 1.0f); // Default value for ambient
			if (AI_SUCCESS == aiGetMaterialColor (mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
				color4_to_float4 (&ambient, c);
			glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, c);

			setArray4f (c, 0.0f, 0.0f, 0.0f, 1.0f); // Like above, for emission
			if (AI_SUCCESS == aiGetMaterialColor (mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
				color4_to_float4 (&emission, c);
			glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, c);

			max = 1;
			ret1 = aiGetMaterialFloatArray (mtl, AI_MATKEY_SHININESS, &shininess, &max);
			if (ret1 == AI_SUCCESS) {
				max = 1;
				ret2 = aiGetMaterialFloatArray (mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
				if (ret2 == AI_SUCCESS)
					glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
				else
					glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);
			}
			else {
				glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
				setArray4f (c, 0.0f, 0.0f, 0.0f, 0.0f);
				glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, c);
			}
			
			/*
			max = 1;
			if ((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
			*/
		}
		
		void generateVertexArray() {
			const aiNode* rootNode = scene->mRootNode;
			aiNode* childNode;
			aiMesh* mesh;
			int i = 0;
			
			numMesh = rootNode->mNumChildren;
			if (numMesh > 0) {
				meshList = new Mesh[numMesh];
			}
			
			if (rootNode->mNumMeshes == 1) {
				mesh = scene->mMeshes[rootNode->mMeshes[0]];
				printf ("Num of faces: %d\n", mesh->mNumFaces);
				printf ("Num of vertices per face: %d\n", mesh->mFaces[0].mNumIndices);
				
				if (mesh->mFaces[0].mNumIndices == 3) {
					rootMesh.faceList = new unsigned int[mesh->mNumFaces * 3];
					rootMesh.vertexArray = new GLfloat[mesh->mNumVertices * 3];
					rootMesh.texCoordArray = new GLfloat[mesh->mNumVertices * 2];
					rootMesh.normalArray = new GLfloat[mesh->mNumVertices * 3];
					
					for (int j = 0; j < mesh->mNumVertices; j ++) {
						rootMesh.vertexArray[j * 3] = mesh->mVertices[j].x;
						rootMesh.vertexArray[j * 3 + 1] = mesh->mVertices[j].y;
						rootMesh.vertexArray[j * 3 + 2] = mesh->mVertices[j].z;
						
						rootMesh.normalArray[j * 3] = mesh->mNormals[j].x;
						rootMesh.normalArray[j * 3 + 1] = mesh->mNormals[j].y;
						rootMesh.normalArray[j * 3 + 2] = mesh->mNormals[j].z;
						
						rootMesh.texCoordArray[j * 2] = mesh->mTextureCoords[0][j].x;
						rootMesh.texCoordArray[j * 2 + 1] = mesh->mTextureCoords[0][j].y;
					}
					
					for (int j = 0; j < mesh->mNumFaces; j ++) {
						rootMesh.faceList[j * 3] = mesh->mFaces[j].mIndices[0];
						rootMesh.faceList[j * 3 + 1] = mesh->mFaces[j].mIndices[1];
						rootMesh.faceList[j * 3 + 2] = mesh->mFaces[j].mIndices[2];
					}
				}
				else {
					rootMesh.faceList = NULL;
				}	
			}
			
			for (i = 0; i < rootNode->mNumChildren; i ++) {
				childNode = rootNode->mChildren[i];
				mesh = scene->mMeshes[childNode->mMeshes[0]];
				
				if (mesh->mFaces[0].mNumIndices == 3) {
					meshList[i].faceList = new unsigned int[mesh->mNumFaces * 3];
					meshList[i].vertexArray = new GLfloat[mesh->mNumVertices * 3];
					meshList[i].texCoordArray = new GLfloat[mesh->mNumVertices * 2];
					meshList[i].normalArray = new GLfloat[mesh->mNumVertices * 3];
					
					for (int j = 0; j < mesh->mNumVertices; j ++) {
						meshList[i].vertexArray[j * 3] = mesh->mVertices[j].x;
						meshList[i].vertexArray[j * 3 + 1] = mesh->mVertices[j].y;
						meshList[i].vertexArray[j * 3 + 2] = mesh->mVertices[j].z;
						
						meshList[i].normalArray[j * 3] = mesh->mNormals[j].x;
						meshList[i].normalArray[j * 3 + 1] = mesh->mNormals[j].y;
						meshList[i].normalArray[j * 3 + 2] = mesh->mNormals[j].z;
						
						meshList[i].texCoordArray[j * 2] = mesh->mTextureCoords[0][j].x;
						meshList[i].texCoordArray[j * 2 + 1] = mesh->mTextureCoords[0][j].y;
					}
					
					for (int j = 0; j < mesh->mNumFaces; j ++) {
						meshList[i].faceList[j * 3] = mesh->mFaces[j].mIndices[0];
						meshList[i].faceList[j * 3 + 1] = mesh->mFaces[j].mIndices[1];
						meshList[i].faceList[j * 3 + 2] = mesh->mFaces[j].mIndices[2];
					}
				}
				else {
					meshList[i].faceList = NULL;
				}
			}
		}
		
		void internalRender() {
			const aiNode* rootNode = scene->mRootNode;
			aiNode* childNode;
			aiMesh* mesh;
			
			if (rootMesh.faceList != NULL) {
				mesh = scene->mMeshes[rootNode->mMeshes[0]];
				
				apply_material(scene->mMaterials[mesh->mMaterialIndex]);
				
				glVertexPointer (3, GL_FLOAT, 0, rootMesh.vertexArray);
				glTexCoordPointer (2, GL_FLOAT, 0, rootMesh.texCoordArray);
				glNormalPointer (GL_FLOAT, 0, rootMesh.normalArray);
				
				glDrawElements (GL_TRIANGLES, mesh->mNumFaces * 3, GL_UNSIGNED_INT, rootMesh.faceList);
			}
			
			for (int i = 0; i < numMesh; i ++) {
				if (meshList[i].faceList != NULL) {
					childNode = rootNode->mChildren[i];
					mesh = scene->mMeshes[childNode->mMeshes[0]];
					
					apply_material(scene->mMaterials[mesh->mMaterialIndex]);
					
					glVertexPointer (3, GL_FLOAT, 0, meshList[i].vertexArray);
					glTexCoordPointer (2, GL_FLOAT, 0, meshList[i].texCoordArray);
					glNormalPointer (GL_FLOAT, 0, meshList[i].normalArray);
					
					glDrawElements (GL_TRIANGLES, mesh->mNumFaces * 3, GL_UNSIGNED_INT, meshList[i].faceList);
				}				
			}
		}
		
		void recursive_render (const aiScene* sc, const aiNode* nd)
		{
			unsigned int i;
			unsigned int n, t;
			aiMatrix4x4 m = nd->mTransformation;

			// update transform
			aiTransposeMatrix4 (&m);
			glPushMatrix();
			glMultMatrixf ((float*) &m);

			// draw all meshes assigned to this node
			for (n = 0; n < nd->mNumMeshes; n ++) {
				const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

				apply_material (sc->mMaterials[mesh->mMaterialIndex]);

				for (t = 0; t < mesh->mNumFaces; ++t) {
					const aiFace* face = &mesh->mFaces[t];
					GLenum face_mode;

					switch (face->mNumIndices) {
						case 1: face_mode = GL_POINTS; break;
						case 2: face_mode = GL_LINES; break;
						case 3: face_mode = GL_TRIANGLES; break;
						default: face_mode = GL_POLYGON; break;
					}

					glBegin (face_mode);

					for(i = 0; i < face->mNumIndices; i++) {
						int index = face->mIndices[i];
						if (mesh->mColors[0] != NULL)
							glColor4fv ((GLfloat*) &mesh->mColors[0][index]);
						if (mesh->mNormals != NULL) 
							glNormal3fv (&mesh->mNormals[index].x);
						if (numTexture > 0 && mesh->HasTextureCoords(0)) {
							glTexCoord2f (mesh->mTextureCoords[0][index].x, mesh->mTextureCoords[0][index].y);
						}
						glVertex3fv (&mesh->mVertices[index].x);
					}

					glEnd();
					
				}

			}

			// draw all children
			for (n = 0; n < nd->mNumChildren; n ++) {
				recursive_render (sc, nd->mChildren[n]);
			}

			glPopMatrix();
		}
		
		void loadTextures() {
			aiString path;
				
			for (int i = 0; i < scene->mNumMaterials; i ++) {
				int texIndex = 0;
				while (AI_SUCCESS == scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path))
				{
					texIndex ++;
					numTexture ++;
				}
			}
			
			if (numTexture > 0) {
				int tex = 0;
				textureArray = new GLuint[numTexture];
				nameToTex = new aiString[scene->mNumMaterials];
				
				glGenTextures (numTexture, textureArray);
				
				for (int i = 0; i < scene->mNumMaterials; i ++) {
					int texIndex = 0;
					while (AI_SUCCESS == scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path))
					{
						aiString prefix;
						prefix = "models\\";
						nameToTex[tex] = path;
						prefix.Append (path.data);
						
						printf ("%s\n", prefix.data);
					
						GLenum format;
						FREE_IMAGE_FORMAT fif = FreeImage_GetFileType (prefix.data, 0);
						FIBITMAP* image = FreeImage_Load (fif, prefix.data, 0);
						FIBITMAP* bitmap = FreeImage_ConvertTo32Bits (image);
						FreeImage_Unload (image);
						
						glBindTexture (GL_TEXTURE_2D, textureArray[tex]);
						glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth (bitmap),
							FreeImage_GetHeight (bitmap), 0, 0x80E1 /* GL_BGRA */, GL_UNSIGNED_BYTE,
							FreeImage_GetBits (bitmap));
							
						FreeImage_Unload (bitmap);
						tex ++;
						
						texIndex ++;
					}
				}
			}
		}
		
		void unloadTextures() {
			glDeleteTextures (numTexture, textureArray);
			delete textureArray;
		}

	public:
		ExternalModel (char filename[], GLfloat h, GLfloat angle) {
			aiString prefix;
			prefix = "models\\";
			prefix.Append (filename);
			scene = aiImportFile (prefix.data,
				aiProcess_Triangulate
				//| aiProcess_PreTransformVertices
				//| aiProcess_SortByPType
				//| aiProcessPreset_TargetRealtime_Quality
				);
			if (!scene) {
				printf ("%s\n", aiGetErrorString());
			}
			
			x = 0;
			y = 0;
			z = 0;
			this->angle = angle;
			
			//generateVertexArray();
			
			numTexture = 0;
			loadTextures();
			
			GLfloat aNum;
			get_bounding_box (&min, &max);
			
			aNum = max.x - min.x;
			aNum = aNum / 2;
			internalX = aNum - max.x;
			min.x = min.x + internalX;
			max.x = max.x + internalX;
			
			internalY = - min.y;
			min.y = 0;
			max.y = max.y + internalY;
			
			aNum = max.z - min.z;
			aNum = aNum / 2;
			internalZ = aNum - max.z;
			min.z = min.z + internalZ;
			max.z = max.z + internalZ;
			
			if (h == 0) {
				scale = 1;
			}
			else {
				scale = h / max.y;
				min.x = min.x * scale;
				max.x = max.x * scale;
				min.y = min.y * scale;
				max.y = max.y * scale;
				min.z = min.z * scale;
				max.z = max.z * scale;
			}
		}
		
		void render() {
			glPushMatrix();
			glTranslatef (x, y, z);
			glRotatef (angle, 0, 1, 0);
			glScalef (scale, scale, scale);
			glTranslatef (internalX, internalY, internalZ);
			//internalRender();
			recursive_render (scene, scene->mRootNode);
			glPopMatrix();
		}
		
		void setScale (GLfloat f) {
			scale = f;
		}
		
		void setPosition (GLfloat x, GLfloat y, GLfloat z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
		
		bool checkValidPosition() {
			if (min.x + x < -6) return false;
			if (max.x + x > +6) return false;
			if (min.z + z < -4) return false;
			if (max.z + z > +4) return false;
			if (y < 0) return false;
			if (max.y + y > +3.6) return false;
			return true;
		}
		
		void translatePosition (GLfloat dx, GLfloat dy, GLfloat dz) {
			GLfloat oldx, oldy, oldz;
			oldx = x; oldy = y; oldz = z;
			x = x + dx;
			y = y + dy;
			z = z + dz;
			if (!checkValidPosition()) {
				x = oldx; y = oldy; z = oldz;
			}
		}
		
		void getBoundingBox (GLfloat & x1, GLfloat & y1, GLfloat & z1,
			GLfloat & x2, GLfloat & y2, GLfloat & z2)
		{
			x1 = min.x + x;
			y1 = y;
			z1 = min.z + z;
			x2 = max.x + x;
			y2 = max.y + y;
			z2 = max.z + z;
		}
		
		~ExternalModel() {
			unloadTextures();
			aiReleaseImport (scene);
		}
	};
	
	ExternalModel* * modelList;
	int modelNum;
	
	void init() {
		FILE* input = fopen ("models.txt", "r");
		char buffer[1024];
		GLfloat height, x, y, z, angle;
		
		fscanf (input, "%d\n", &modelNum);
		modelList = new ExternalModel*[modelNum];
		for (int i = 0; i < modelNum; i ++) {
			fscanf (input, "%s\n", buffer);
			fscanf (input, "%f %f %f\n", &x, &y, &z);
			fscanf (input, "%f %f\n", &height, &angle);
			modelList[i] = new ExternalModel (buffer, height, angle);
			modelList[i]->setPosition (x, y, z);
		}
		fclose (input);
	}
	
	void render() {
		glEnableClientState (GL_NORMAL_ARRAY);
		for (int i = 0; i < modelNum; i ++) {
			modelList[i]->render();
		}
		glDisableClientState (GL_NORMAL_ARRAY);
	}
	
	void cleanUp() {
		for (int i = 0; i < modelNum; i ++) {
			delete modelList[i];
		}
		delete modelList;
	}
}