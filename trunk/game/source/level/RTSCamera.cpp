/** 
 * \file RTSCamera.cpp
 * \brief File RTSCamera.cpp is taken from irrlicht forum. RTS Camera class is created by CmdKewin.
 * link: http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=17066&start=0
 * I changed that camera code little bit to suite my needs.
 * Big thanks to CmdKewin.
 */

#include "RTSCamera.h"

RTSCamera::RTSCamera(IrrlichtDevice* devicepointer,ISceneNode* parent,ISceneManager* smgr,s32 id,
    f32 rs,f32 zs,f32 ts)
   : ICameraSceneNode(parent,smgr,id,vector3df(1.0f,1.0f,1.0f),vector3df(0.0f,0.0f,0.0f),
               vector3df(1.0f,1.0f,1.0f)),InputReceiverEnabled(true)
{
   device = devicepointer;
   BBox.reset(0,0,0);

   UpVector.set(0.0f, 1.0f, 0.0f);

   // set default projection
   Fovy = core::PI / 2.5f;   // Field of view, in radians.
   Aspect = 4.0f / 3.0f;   // Aspect ratio.
   ZNear = 1.0f;      // value of the near view-plane.
   ZFar = 100000.0f;      // Z-value of the far view-plane.
   oldTimeMs = 0;

   IVideoDriver* d = smgr->getVideoDriver();
   if (d)
      Aspect = (f32)d->getCurrentRenderTargetSize().Width / (f32)d->getCurrentRenderTargetSize().Height;

   zooming = false;
   rotating = false;
   moving = false;
   translating = false;
   zoomSpeed = zs;
   rotateSpeed = rs;
   translateSpeed = ts;
   targetMinDistance = 1.0f;
   targetMaxDistance = 900.0f;
   Target.set(0.0f,0.0f,0.0f);
   rotX = 0;
   rotY = 0;
   oldTarget = Target;
   maxRot = 0;

   atMinDistance = false;
   atMaxDistance = false;

   allKeysUp();
   allMouseButtonsUp();

   recalculateProjectionMatrix();
   recalculateViewArea();

   smgr->setActiveCamera(this);
}

RTSCamera::~RTSCamera()
{
}

void RTSCamera::setRotation(const vector3df& rotation)
{
	Rotation = rotation;
}

//! Sets the projection matrix of the camera.
/** The core::matrix4 class has some methods
to build a projection matrix. e.g: core::matrix4::buildProjectionMatrixPerspectiveFovLH
\param projection: The new projection matrix of the camera. */
void RTSCamera::setProjectionMatrix(const core::matrix4& projection, bool isOrthogonal)
{
	IsOrthogonal = isOrthogonal;
	ViewArea.getTransform ( video::ETS_PROJECTION ) = projection;
}

//! Set the binding between the camera's rotation adn target.
void RTSCamera::bindTargetAndRotation(bool bound)
{
	TargetAndRotationAreBound = bound;
}


//! Gets the binding between the camera's rotation and target.
bool RTSCamera::getTargetAndRotationBinding(void) const
{
	return TargetAndRotationAreBound;
}

void RTSCamera::ResetMouseKeys()
{
	MouseKeys[0] = false;
	MouseKeys[1] = false;
	MouseKeys[2] = false;
}

bool RTSCamera::OnEvent(const SEvent& event)
{
	return false;
}

//Uvent?
//Well, OnEvent is called by Irrlicht engine, cause camera is inheriting ICameraSceneNode
//And I want to call OnEvent from my game engine loop, So i call OnUvent, and OnEvent is ignored.
bool RTSCamera::OnUvent(const SEvent& event)
{
   if (!InputReceiverEnabled)
      return false;

   dimension2d<u32> ssize = SceneManager->getVideoDriver()->getScreenSize();
   if(event.EventType == EET_MOUSE_INPUT_EVENT)
   {
      switch(event.MouseInput.Event)
      {
         case EMIE_RMOUSE_PRESSED_DOWN:
            MouseKeys[2] = true;
            break;
         case EMIE_MMOUSE_PRESSED_DOWN:
            MouseKeys[1] = true;
            break;
         case EMIE_LMOUSE_LEFT_UP:
            MouseKeys[0] = false;
           break;
         case EMIE_RMOUSE_LEFT_UP:
            MouseKeys[2] = false;
            break;
         case EMIE_MMOUSE_LEFT_UP:
            MouseKeys[1] = false;
            break;
         case EMIE_MOUSE_MOVED:
            {
               IVideoDriver* driver = SceneManager->getVideoDriver();
               if (driver)
               {
                  MousePos.X = event.MouseInput.X / (f32)ssize.Width;
                  MousePos.Y = event.MouseInput.Y / (f32)ssize.Height;
               }
            }
            break;
         case EMIE_MOUSE_WHEEL:
			 {
#ifdef _IRR_OSX_PLATFORM_
				 if (!Keys[KEY_KEY_C])
#else
				if (!Keys[KEY_SHIFT] && !Keys[KEY_RSHIFT] && !Keys[KEY_LSHIFT])
#endif
				 {
					currentZoom -= event.MouseInput.Wheel * zoomSpeed;
					if (currentZoom < targetMinDistance)
						currentZoom = targetMinDistance;
					else if (currentZoom > targetMaxDistance)
						currentZoom = targetMaxDistance;
				 }
			 }
            break;
         default:
            break;
      }

      return true;
   }

   /*if(event.EventType == EET_KEY_INPUT_EVENT)
   {
	   //Bug in Irrlicht 1.6 Issue no. X: When shif is pressed, we get press down event for KEY_RSHIFT, and release event for KEY_SHIFT - phuckers
	   if(!event.KeyInput.PressedDown) {
			if (event.KeyInput.Key == KEY_SHIFT)
			{
				Keys[KEY_SHIFT] = false;
				Keys[KEY_RSHIFT] = false;
				Keys[KEY_LSHIFT] = false;
			}
	   }
      Keys[event.KeyInput.Key] = event.KeyInput.PressedDown;
      return true;
   }*/

   return false;
}

void RTSCamera::OnRegisterSceneNode()
{
   vector3df pos = getAbsolutePosition();
   vector3df tgtv = Target - pos;
   tgtv.normalize();

   vector3df up = UpVector;
   up.normalize();

   f32 dp = tgtv.dotProduct(up);
	f32 adp =  fabs ( dp ); 
	f32 o = 1;

   if ( core::equals (adp, o ) )
   {
      up.X += 0.5f;
   }

   ViewArea.getTransform(video::ETS_VIEW).buildCameraLookAtMatrixLH(pos, Target, up);
   ViewArea.getTransform(video::ETS_VIEW) *= Affector;

   recalculateViewArea();

   if( SceneManager->getActiveCamera () == this )
      SceneManager->registerNodeForRendering(this, ESNRP_CAMERA);

   ISceneNode::OnRegisterSceneNode();
}

void RTSCamera::render()
{
   IVideoDriver* driver = SceneManager->getVideoDriver();
   if ( driver)
   {
        driver->setTransform(video::ETS_PROJECTION, ViewArea.getTransform ( video::ETS_PROJECTION) );
		driver->setTransform(video::ETS_VIEW, ViewArea.getTransform ( video::ETS_VIEW) );
   }
}

void RTSCamera::OnAnimate(u32 timeMs)
{
	//subtract new time from old time to get elapsed time :)
	u32 elapsed = timeMs - oldTimeMs;
	oldTimeMs = timeMs;
	//if (elapsed == 0) elapsed = 1;
	animate(elapsed);

   setPosition(Pos);
   updateAbsolutePosition();
}

void RTSCamera::setInputReceiverEnabled(bool enabled)
{
   InputReceiverEnabled = enabled;
}

bool RTSCamera::isInputReceiverEnabled() const
{
   _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
   return InputReceiverEnabled;
}

const aabbox3d<f32>& RTSCamera::getBoundingBox() const
{
   return ViewArea.getBoundingBox();
}

const matrix4& RTSCamera::getProjectionMatrix() const
{
   return ViewArea.getTransform ( video::ETS_PROJECTION );
}

const SViewFrustum* RTSCamera::getViewFrustum() const
{
   return &ViewArea;
}

const vector3df& RTSCamera::getTarget() const
{
   return Target;
}

const matrix4& RTSCamera::getViewMatrix() const
{
   return ViewArea.getTransform ( video::ETS_VIEW );
}

const vector3df& RTSCamera::getUpVector() const
{
   return UpVector;
}

f32 RTSCamera::getNearValue() const
{
   return ZNear;
}

f32 RTSCamera::getFarValue() const
{
   return ZFar;
}

f32 RTSCamera::getAspectRatio() const
{
   return Aspect;
}

f32 RTSCamera::getFOV() const
{
   return Fovy;
}

void RTSCamera::setNearValue(f32 f)
{
   ZNear = f;
   recalculateProjectionMatrix();
}

void RTSCamera::setFarValue(f32 f)
{
   ZFar = f;
   recalculateProjectionMatrix();
}

void RTSCamera::setAspectRatio(f32 f)
{
   Aspect = f;
   recalculateProjectionMatrix();
}

void RTSCamera::setFOV(f32 f)
{
   Fovy = f;
   recalculateProjectionMatrix();
}

void RTSCamera::setUpVector(const vector3df& pos)
{
   UpVector = pos;
}

void RTSCamera::setProjectionMatrix(const matrix4& projection)
{
   ViewArea.getTransform ( video::ETS_PROJECTION ) = projection;
}

//! Sets a custom view matrix affector.
/** The matrix passed here, will be multiplied with the view
matrix when it gets updated. This allows for custom camera
setups like, for example, a reflection camera.
\param affector The affector matrix. */
void RTSCamera::setViewMatrixAffector(const core::matrix4& affector)
{
	Affector = affector;
}

//! Get the custom view matrix affector.
/** \return The affector matrix. */
const matrix4& RTSCamera::getViewMatrixAffector() const
{
	return Affector;
}

void RTSCamera::setPosition(const core::vector3df& pos)
{
   Pos = pos;
   updateAnimationState();

   ISceneNode::setPosition(pos);
}

void RTSCamera::setTarget(const core::vector3df& pos)
{
   Target = oldTarget = pos;
   updateAnimationState();
}

void RTSCamera::setZoomSpeed(f32 value)
{
   zoomSpeed = value;
}

void RTSCamera::setTranslateSpeed(f32 value)
{
   translateSpeed = value;
}

void RTSCamera::setRotationSpeed(f32 value)
{
   rotateSpeed = value;
}

void RTSCamera::pointCameraAtNode(ISceneNode* selectednode)
{
   vector3df totarget = getPosition() - getTarget();
   setPosition(selectednode->getPosition() + (totarget.normalize() * 600));
   setTarget(selectednode->getPosition());
   updateAnimationState();
}

void RTSCamera::setMinZoom(f32 amount)
{
   targetMinDistance = amount;
}

void RTSCamera::setMaxZoom(f32 amount)
{
   targetMaxDistance = amount;
}

void RTSCamera::recalculateProjectionMatrix()
{
   ViewArea.getTransform ( video::ETS_PROJECTION ).buildProjectionMatrixPerspectiveFovLH(Fovy, Aspect, ZNear, ZFar);
}


void RTSCamera::recalculateViewArea()
{
    ViewArea.cameraPosition = getAbsolutePosition();

	core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
	m.setbyproduct_nocheck(ViewArea.getTransform(video::ETS_PROJECTION),
						ViewArea.getTransform(video::ETS_VIEW));
	ViewArea.setFrom(m);

}

void RTSCamera::allKeysUp()
{
   for(int i = 0;i < KEY_KEY_CODES_COUNT;i++)
      Keys[i] = false;
}

void RTSCamera::allMouseButtonsUp()
{
   for (s32 i=0; i<3; ++i)
      MouseKeys[i] = false;
}

bool RTSCamera::isKeyDown(s32 key)
{
   return Keys[key];
}

bool RTSCamera::isMouseButtonDown(s32 key)
{
   return MouseKeys[key];
}

void RTSCamera::animate(u32 elapsed)
{
	bool movebymouse = false;
   f32 nRotX = rotX;
   f32 nRotY = rotY;
   f32 nZoom = currentZoom;

   vector3df translate(oldTarget);
   vector3df tvectX = Pos - Target;
   tvectX = tvectX.crossProduct(UpVector);
   tvectX.normalize();

//   if(oldTarget.X != 0)
//   {
//   }

   //Zoom
   if (isMouseButtonDown(MOUSE_BUTTON_RIGHT) && isMouseButtonDown(MOUSE_BUTTON_LEFT))
   {
      if (!zooming)
      {
         zoomStartX = MousePos.X;
         zoomStartY = MousePos.Y;
         zooming = true;
         nZoom = currentZoom;
      }
      else
      {
         f32 old = nZoom;
         nZoom += (zoomStartX - MousePos.X) * zoomSpeed * 100;

//         f32 targetMinDistance = 0.1f;
//         if (nZoom < targetMinDistance)
//            nZoom = targetMinDistance;

         if (nZoom < targetMinDistance)
            nZoom = targetMinDistance;
         else if (nZoom > targetMaxDistance)
            nZoom = targetMaxDistance;


         if (nZoom < 0)
            nZoom = old;
      }
   }
   else
   {
      if (zooming)
      {
         f32 old = currentZoom;
         currentZoom = currentZoom + (zoomStartX - MousePos.X ) * zoomSpeed;
         nZoom = currentZoom;

         if (nZoom < 0)
            nZoom = currentZoom = old;
      }
      zooming = false;
   }

   //We rotate in Editor with right mouse button
   if(isMouseButtonDown(MOUSE_BUTTON_RIGHT) && !isKeyDown(KEY_SHIFT) && !isKeyDown(KEY_LSHIFT) && !isKeyDown(KEY_RSHIFT))
   {
	   movebymouse = true;
      if (!rotating)
      {
         rotateStartX = MousePos.X;
         rotateStartY = MousePos.Y;
         rotating = true;
         //nRotX = rotX;
         //nRotY = rotY;
      }
      else
      {
         nRotX -= (rotateStartX - MousePos.X) * rotateSpeed * (f32)elapsed/500;
         nRotY -= (rotateStartY - MousePos.Y) * rotateSpeed * (f32)elapsed/500;
		 rotX = nRotX;//rotX - (rotateStartX - MousePos.X) * rotateSpeed * (f32)elapsed/500;
         rotY = nRotY;//rotY - (rotateStartY - MousePos.Y) * rotateSpeed * (f32)elapsed/500;
		 if(rotY < -350)
		 {
			 rotY = -350;
			 nRotY = -350;
		 }
		 if(rotY > -280)
		 {
			 rotY = -280;
			 nRotY = -280;
		 }
		 printf("PERA CAMERA:%f\n", rotY);
      }
   }
   else
   {
      if (rotating)
      {
         rotX = rotX - (rotateStartX - MousePos.X) * rotateSpeed * (f32)elapsed/500;
         rotY = rotY - (rotateStartY - MousePos.Y) * rotateSpeed * (f32)elapsed/500;
         nRotX = rotX;
         nRotY = rotY;
      }

      rotating = false;
   }

   //Translate
   if(isMouseButtonDown(MOUSE_BUTTON_RIGHT) && (isKeyDown(KEY_SHIFT) || isKeyDown(KEY_LSHIFT) || isKeyDown(KEY_RSHIFT)))
   {
	    movebymouse = true;
      if (!translating)
      {
         translateStartX = MousePos.X;
         translateStartY = MousePos.Y;
         translating = true;
      }
      else
      {
         translate += tvectX * (translateStartX - MousePos.X) * translateSpeed * (f32)elapsed/100;
         translate.X += tvectX.Z * (translateStartY - MousePos.Y) * translateSpeed * (f32)elapsed/100;
         translate.Z -= tvectX.X * (translateStartY - MousePos.Y) * translateSpeed * (f32)elapsed/100;

         oldTarget = translate;
      }
   }
   else if (isKeyDown(KEY_KEY_W) || isKeyDown(KEY_UP) && !zooming && !isKeyDown(KEY_SHIFT) && !isKeyDown(KEY_LSHIFT) && !isKeyDown(KEY_RSHIFT))
   {
      if (!translating)
         translating = true;
      else
      {
         vector3df movevector = getPosition() - getTarget();
         movevector.Y = 0;
         movevector.normalize();

          translate =getTarget() - movevector * translateSpeed * (f32)elapsed/100;
         oldTarget = translate;
         //setPosition(getPosition() - movevector * translateSpeed * (f32)elapsed/100);
         //setTarget(getTarget() - movevector * translateSpeed * (f32)elapsed/100);
         //updateAbsolutePosition();
      }
   }
   else if (isKeyDown(KEY_KEY_S) || isKeyDown(KEY_DOWN) && !zooming && !isKeyDown(KEY_SHIFT) && !isKeyDown(KEY_LSHIFT) && !isKeyDown(KEY_RSHIFT))
   {
      if (!translating)
         translating = true;
      else
      {
         vector3df movevector = getPosition() - getTarget();
         movevector.Y = 0;
         movevector.normalize();

          translate =getTarget() + movevector * translateSpeed * (f32)elapsed/100;
         oldTarget = translate;
//        setPosition(getPosition() + movevector * translateSpeed * (f32)elapsed/100);
  //       setTarget(getTarget() + movevector * translateSpeed * (f32)elapsed/100);
    //     updateAbsolutePosition();
      }
   }
   else if (isKeyDown(KEY_KEY_A) || isKeyDown(KEY_LEFT) && !zooming && !isKeyDown(KEY_SHIFT) && !isKeyDown(KEY_LSHIFT) && !isKeyDown(KEY_RSHIFT))
   {
      if (!translating)
         translating = true;
      else
      {
         /*translate += tvectX * (translateStartX - MousePos.X) * translateSpeed * (f32)elapsed/100;
         translate.X += tvectX.Z * (translateStartY - MousePos.Y) * translateSpeed * (f32)elapsed/100;
         translate.Z -= tvectX.X * (translateStartY - MousePos.Y) * translateSpeed * (f32)elapsed/100;
*/
          //= translate;


		 vector3df totargetvector = getPosition() - getTarget();
         totargetvector.normalize();
         vector3df crossvector = totargetvector.crossProduct(getUpVector());
         vector3df strafevector = crossvector.normalize();

         translate =getTarget() - strafevector * translateSpeed * (f32)elapsed/100;
         oldTarget = translate;
         //setPosition(getPosition() - strafevector * translateSpeed * (f32)elapsed/100);
         //setTarget(getTarget() - strafevector * translateSpeed * (f32)elapsed/100);
		 //oldTarget = getTarget();
         //updateAbsolutePosition();
      }
   }
   else if (isKeyDown(KEY_KEY_D) || isKeyDown(KEY_RIGHT) && !zooming && !isKeyDown(KEY_SHIFT) && !isKeyDown(KEY_LSHIFT) && !isKeyDown(KEY_RSHIFT))
   {
      if (!translating)
         translating = true;
      else
      {
         vector3df totargetvector = getPosition() - getTarget();
         totargetvector.normalize();
         vector3df crossvector = totargetvector.crossProduct(getUpVector());
         vector3df strafevector = crossvector.normalize();

         translate =getTarget() + strafevector * translateSpeed * (f32)elapsed/100;
         oldTarget = translate;
        //setPosition(getPosition() + strafevector * translateSpeed * (f32)elapsed/100);
         //setTarget(getTarget() + strafevector * translateSpeed * (f32)elapsed/100);
         //updateAbsolutePosition();
      }
   }
   else
   {
      translating = false;

	  /*moving camera when mouse on screen edges*/
      /*if (!translating && !zooming && !rotating)
      {
         //Mouse Coordinates go from 0 to 1 on both axes
         if (MousePos.X < 0.05)   //Up
         {
            vector3df totargetvector = getPosition() - getTarget();
            totargetvector.normalize();
            vector3df crossvector = totargetvector.crossProduct(getUpVector());
            vector3df strafevector = crossvector.normalize();

            setPosition(getPosition() - strafevector * translateSpeed);
            setTarget(getTarget() - strafevector * translateSpeed);
            updateAbsolutePosition();
         }
         else if (MousePos.X > 0.95) //Down
         {
            vector3df totargetvector = getPosition() - getTarget();
            totargetvector.normalize();
            vector3df crossvector = totargetvector.crossProduct(getUpVector());
            vector3df strafevector = crossvector.normalize();

            setPosition(getPosition() + strafevector * translateSpeed);
            setTarget(getTarget() + strafevector * translateSpeed);
            updateAbsolutePosition();
         }
         else if (MousePos.Y < 0.05)   //Up
         {
            vector3df movevector = getPosition() - getTarget();
            movevector.Y = 0;
            movevector.normalize();

            setPosition(getPosition() - movevector * translateSpeed);
            setTarget(getTarget() - movevector * translateSpeed);
            updateAbsolutePosition();
         }
         else if (MousePos.Y > 0.95) //Down
         {
            vector3df movevector = getPosition() - getTarget();
            movevector.Y = 0;
            movevector.normalize();

            setPosition(getPosition() + movevector * translateSpeed);
            setTarget(getTarget() + movevector * translateSpeed);
            updateAbsolutePosition();
         }
      }*/
   }

   //Set Position
   //if(movebymouse)
   //{
		Target = translate;

		Pos.X = nZoom + Target.X;
		Pos.Y = Target.Y;
		Pos.Z = Target.Z;

		Pos.rotateXYBy(nRotY, Target);
		Pos.rotateXZBy(-nRotX, Target);

		//Correct Rotation Error
		UpVector.set(0,1,0);
		UpVector.rotateXYBy(-nRotY, core::vector3df(0,0,0));
		UpVector.rotateXZBy(-nRotX+180.f, core::vector3df(0,0,0));
   //}
}

void RTSCamera::updateAnimationState()
{
   vector3df pos(Pos - Target);

   // X rotation
   vector2df vec2d(pos.X, pos.Z);
   rotX = (f32)vec2d.getAngle();

   // Y rotation
   pos.rotateXZBy(rotX, vector3df());
   vec2d.set(pos.X, pos.Y);
   rotY = -(f32)vec2d.getAngle();

   // Zoom
   currentZoom = (f32)Pos.getDistanceFrom(Target);
}