void setArray4f (GLfloat array[], GLfloat x1, GLfloat x2, GLfloat x3, GLfloat x4) {
	array[0] = x1;
	array[1] = x2;
	array[2] = x3;
	array[3] = x4;
}

namespace Walls {
	GLuint * textureArray;
	int textureNum;

	class Wall {
	private:
		GLfloat x1, z1, x2, z2, h;
		GLfloat normalX, normalZ;
		
		GLfloat matDiff[4], matSpec[4], matShininess;
		GLint textureId;
		
		GLfloat * vertexArray;
		GLfloat * texCoordArray;
		GLushort * quadArray;
		GLint vertexNum, quadNum;
		
		bool isFloor;
		
		void generateVertexArray (GLfloat elementSize) {
			GLfloat xWidth = (x2 - x1);
			GLfloat zWidth = (z2 - z1);
			GLfloat uWidth = sqrt((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1));
			
			GLfloat nW = uWidth / elementSize;
			GLfloat nH = h / elementSize;
			GLfloat elementSizeX = xWidth / floor(nW);
			GLfloat elementSizeZ = zWidth / floor(nW);
			
			GLfloat i, j;
			int k, m, n;
			
			vertexNum = (ceil(nW) + 1) * (ceil(nH) + 1);
			quadNum = ceil(nW) * ceil(nH); 
			vertexArray = new GLfloat[vertexNum * 3];
			texCoordArray = new GLfloat[vertexNum * 2];
			quadArray = new GLushort[quadNum * 4];
			
			k = 0;
			for (j = 0; j < nH; j = j + 1) {
				for (i = 0; i < nW; i = i + 1) {
					vertexArray[k * 3] = x1 + i * elementSizeX;
					vertexArray[k * 3 + 1] = j * elementSize;
					vertexArray[k * 3 + 2] = z1 + i * elementSizeZ;
					texCoordArray[k * 2] = i * elementSize / 4;
					texCoordArray[k * 2 + 1] = j * elementSize / 4;
					k ++;
				}
				vertexArray[k * 3] = x2;
				vertexArray[k * 3 + 1] = j * elementSize;
				vertexArray[k * 3 + 2] = z2;
				texCoordArray[k * 2] = uWidth / 4;
				texCoordArray[k * 2 + 1] = j * elementSize / 4;			
				k ++;
			}
			for (i = 0; i < nW; i = i + 1) {
				vertexArray[k * 3] = x1 + i * elementSizeX;
				vertexArray[k * 3 + 1] = h;
				vertexArray[k * 3 + 2] = z1 + i * elementSizeZ;
				texCoordArray[k * 2] = i * elementSize / 4;
				texCoordArray[k * 2 + 1] = h / 4;
				k ++;
			}
			vertexArray[k * 3] = x2;
			vertexArray[k * 3 + 1] = h;
			vertexArray[k * 3 + 2] = z2;
			texCoordArray[k * 2] = uWidth / 4;
			texCoordArray[k * 2 + 1] = h / 4;
			
			k = 0;
			for (m = 0; m < nH; m ++) {
				for (n = 0; n < nW; n ++) {
					quadArray[k * 4] = m * (ceil(nW) + 1) + n;
					quadArray[k * 4 + 1] = m * (ceil(nW) + 1) + n + 1;
					quadArray[k * 4 + 2] = (m + 1) * (ceil(nW) + 1) + n + 1;
					quadArray[k * 4 + 3] = (m + 1) * (ceil(nW) + 1) + n;
					k ++;
				}
			}
		}
		
		void generateVertexArrayForFloor (GLfloat elementSize) {
			GLfloat xWidth = (x2 - x1);
			GLfloat zHeight = (z2 - z1);
			
			GLfloat nW = xWidth / elementSize;
			GLfloat nH = zHeight / elementSize;
			
			GLfloat i, j;
			int k, m, n;
			
			vertexNum = (ceil(nW) + 1) * (ceil(nH) + 1);
			quadNum = ceil(nW) * ceil(nH); 
			vertexArray = new GLfloat[vertexNum * 3];
			texCoordArray = new GLfloat[vertexNum * 2];
			quadArray = new GLushort[quadNum * 4];
			
			k = 0;
			for (j = 0; j < nH; j = j + 1) {
				for (i = 0; i < nW; i = i + 1) {
					vertexArray[k * 3] = x1 + i * elementSize;
					vertexArray[k * 3 + 1] = h;
					vertexArray[k * 3 + 2] = z1 + j * elementSize;
					texCoordArray[k * 2] = i * elementSize / 4;
					texCoordArray[k * 2 + 1] = j * elementSize / 4;
					k ++;
				}
				vertexArray[k * 3] = x2;
				vertexArray[k * 3 + 1] = h;
				vertexArray[k * 3 + 2] = z1 + j * elementSize;;
				texCoordArray[k * 2] = xWidth / 4;
				texCoordArray[k * 2 + 1] = j * elementSize / 4;			
				k ++;
			}
			for (i = 0; i < nW; i = i + 1) {
				vertexArray[k * 3] = x1 + i * elementSize;
				vertexArray[k * 3 + 1] = h;
				vertexArray[k * 3 + 2] = z2;
				texCoordArray[k * 2] = i * elementSize / 4;
				texCoordArray[k * 2 + 1] = zHeight / 4;
				k ++;
			}
			vertexArray[k * 3] = x2;
			vertexArray[k * 3 + 1] = h;
			vertexArray[k * 3 + 2] = z2;
			texCoordArray[k * 2] = xWidth / 4;
			texCoordArray[k * 2 + 1] = zHeight / 4;
			
			k = 0;
			for (m = 0; m < nH; m ++) {
				for (n = 0; n < nW; n ++) {
					quadArray[k * 4] = m * (ceil(nW) + 1) + n;
					quadArray[k * 4 + 1] = m * (ceil(nW) + 1) + n + 1;
					quadArray[k * 4 + 2] = (m + 1) * (ceil(nW) + 1) + n + 1;
					quadArray[k * 4 + 3] = (m + 1) * (ceil(nW) + 1) + n;
					k ++;
				}
			}
		}
		
	public:
		Wall (GLfloat x1, GLfloat z1, GLfloat x2, GLfloat z2, GLfloat h, bool isFloor, GLuint textureId) {
			this->x1 = x1;
			this->z1 = z1;
			this->x2 = x2;
			this->z2 = z2;
			this->h = h;
			this->isFloor = isFloor;
			this->textureId = textureId;
			
			setArray4f (matDiff, 1, 1, 1, 1);
			setArray4f (matSpec, 0.5, 0.5, 0.5, 0.5);
			matShininess = 5;
			
			normalX = - (z2 - z1);
			normalZ = (x2 - x1);
			
			if (!isFloor)
				generateVertexArray (0.125);
			else 
				generateVertexArrayForFloor (0.125);
		}

		void render() {
			glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matDiff);
			glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
			glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
			
			glBindTexture (GL_TEXTURE_2D, textureArray[textureId]);
			glVertexPointer (3, GL_FLOAT, 0, vertexArray);
			glTexCoordPointer (2, GL_FLOAT, 0, texCoordArray);
			if (isFloor) {
				glNormal3f (0, -1, 0);
			}
			else {	
				glNormal3f (normalX, 0, normalZ);
			}
			glDrawElements (GL_QUADS, quadNum * 4, GL_UNSIGNED_SHORT, quadArray);
		}
		
		~Wall() {
			delete vertexArray;
			delete texCoordArray;
			delete quadArray;
		}
	};
	
	Wall* * wallArray;
	Wall* * floorArray;
	int wallNum, floorNum;
	
	void init() {
		FILE* input = fopen ("wall.txt", "r");
		char buffer[1024];
		
		fscanf (input, "%1023s\n", buffer);
		if (strcmp(buffer, "#Texture") == 0) {
			fscanf (input, "%d\n", &textureNum);
			textureArray = new GLuint[textureNum];
			
			glGenTextures (textureNum, textureArray);
			for (int i = 0; i < textureNum; i ++) {
				fscanf (input, "%1023s\n", buffer);
				
				FREE_IMAGE_FORMAT fif = FreeImage_GetFileType (buffer, 0);
				FIBITMAP* image = FreeImage_Load (fif, buffer, 0);
				FIBITMAP* bitmap = FreeImage_ConvertTo32Bits (image);
				FreeImage_Unload (image);
				
				glBindTexture (GL_TEXTURE_2D, textureArray[i]);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth (bitmap),
					FreeImage_GetHeight (bitmap), 0, 0x80E1 /* GL_BGRA */, GL_UNSIGNED_BYTE,
					FreeImage_GetBits (bitmap));
					
				FreeImage_Unload (bitmap);
			}
		}
		
		fscanf (input, "%1023s\n", buffer);
		if (strcmp(buffer, "#Wall") == 0) {
			GLfloat x1, x2, z1, z2, h;
			GLuint tex;
		
			fscanf (input, "%d\n", &wallNum);
			wallArray = new Wall*[wallNum];
			for (int i = 0; i < wallNum; i ++) {
				fscanf (input, "%f %f %f %f %f %d\n", &x1, &z1, &x2, &z2, &h, &tex);
				wallArray[i] = new Wall(x1, z1, x2, z2, h, false, tex);
			}
		}
		
		fscanf (input, "%1023s\n", buffer);
		if (strcmp(buffer, "#Floor") == 0) {
			GLfloat x1, x2, z1, z2, h;
			GLuint tex;
		
			fscanf (input, "%d\n", &floorNum);
			floorArray = new Wall*[floorNum];
			for (int i = 0; i < floorNum; i ++) {
				fscanf (input, "%f %f %f %f %f %d\n", &x1, &z1, &x2, &z2, &h, &tex); 
				floorArray[i] = new Wall(x1, z1, x2, z2, h, true, tex);
			}
		}
		
		fclose (input);
	}
	
	void cleanUp() {
		if (textureNum > 0) {
			glDeleteTextures (textureNum, textureArray);
		}
		delete textureArray;
		
		for (int i = 0; i < wallNum; i ++) {
			delete wallArray[i];
		}
		delete wallArray;
		
		for (int i = 0; i < floorNum; i ++) {
			delete floorArray[i];
		}
		delete floorArray;
	}
	
	void render() {
		//glDisable (GL_CULL_FACE);
		//glEnable (GL_TEXTURE);
		for (int i = 0; i < wallNum; i ++) {
			wallArray[i]->render();
		}
		for (int i = 0; i < floorNum; i ++) {
			floorArray[i]->render();
		}
	}
}