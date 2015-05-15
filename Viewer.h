class Viewer {
private:
	float camX, camY, camZ;
	float rayX, rayY, rayZ;
	float upAngle;
public:
	float spinX, spin;

	Viewer(){
		camX = 0;	camZ = 0;	camY = 1.6;
		spin = PI; spinX = 0;
		rayX = sin(spin); rayY = sin(spinX); rayZ = cos(spin);
		upAngle = 0;
	}
	void setPosition(float camX,float camY,float camZ){
		this->camX = camX; 
		this->camY = camY; 
		this->camZ = camZ; 
	}
	bool checkValidPosition() {
		if (camX < -5.7) return false;
		if (camX > +5.7) return false;
		if (camZ < -3.7) return false;
		if (camZ > +3.7) return false;
		return true;
	}
	void translatePosition (float x, float y, float z) {
		GLfloat oldX = camX;
		GLfloat oldZ = camZ;
		camX = camX + x;
		camY = camY + y;
		camZ = camZ + z;
		if (!checkValidPosition()) {
			camX = oldX; camZ = oldZ;
		}
	}
	void setDirection(float x, float y, float z){
		rayX = x;
		rayY = y;
		rayZ = z;
	}
	void setUpDirection(float angle){
		upAngle = angle;
	}

	void getDirection(float &x, float &y, float &z){
		float nomal = sqrt(rayX*rayX + rayY*rayY + rayZ*rayZ);
		x = rayX/nomal; y = rayY/nomal; z = rayZ/nomal;
	}

	void applyCamera(){
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(camX, camY, camZ, camX+rayX, camY+rayY, camZ+rayZ, sin(upAngle), cos(upAngle), 0);
	}
	
	// ------------------------------------------------------------------------- //
	float lineFunc(float x,float z) {
		printf ("rayX: %f    rayZ: %f    camX: %f    camZ: %f\n", rayX, rayZ, camX, camZ);
		printf ("x: %f    z: %f\n\n", x, z);
		return -rayZ*(x - camX) + rayX*(z-camZ);
	}
	bool ray(float x1, float z1, float x2, float z2, float y1, float y2) {
		float a1, b1, c1, a2, b2, c2, h;
		float solX, solZ, lineA, lineB;
		float vecX, vecZ;
		float theta, cosin;
		
		lineA = lineFunc(x1, z1);
		lineB = lineFunc(x2, z2);
		if(lineA * lineB < 0) {
			a1 = -rayZ; b1 = rayX; c1 = a1*camX + b1*camZ;
			b2 = x2 - x1; a2 = -z2+z1; c2 = a2*x1 + b2*z1;
			// giao diem
			solZ = (c2*a1 - c1*a2)/(a1*b2 - a2*b1);
			solX = (c2*b1 - c1*b2)/(a2*b1 - a1*b2);
			//
			float tx = (solX - camX)/rayX;
			float tz = (solZ - camZ)/rayZ;
			h = camY + tx*rayY;	
			if(h > y1 && h < y2) {
				return true;
			}else{
				return false;
			}
		}else{
			return false;
		}
	}

	bool tracing(float x1, float y1, float z1, float x2, float y2, float z2) {
		if(ray(x1, z1, x1,z2, y1, y2)){
			return true;
		}
		if(ray(x1, z1, x2,z1,y1,y2)){
			return true;
		}
		if(ray(x2,z2, x1,z2, y1, y2)){
			return true;
		}
		if(ray(x2, z2, x2, z1, y1, y2)){
			return true;
		}
		return false;
	}
	
	void render() {
		glPointSize (6);
		glDisable (GL_LIGHTING);
		glColor3f (1, 0, 0);
		glBegin(GL_POINTS);
		glVertex3f (camX + rayX, camY + rayY, camZ + rayZ);
		glEnd();
		glEnable (GL_LIGHTING);
	}
};