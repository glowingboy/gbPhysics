#include "matrix.h"

#include "boundingbox.h"

GB_PHYSICS_NS_BEGIN

// symmetric frustum
template <typename T>
struct frustum
{
    /*
      perspective frustum
       	       l---------------r
      		\      |      /
		 \     |     /
		  \    |    /
		   \ near  /
      		    \  |  /
		     \ | /
      		      \|/
		      fov

      r = near * tan(fov/2)
      l = -r;

      since aspectRatio = height / width
      so t = r * aspectRatio;
      b = -t;
    */
    set(const float fov,
	    const float aspectRatio,
	    const float clipNear_,
	    const float clipFar_)
    {
	clipNear = clipNear_;
	clipFar = clipFar_;
	
	right =  clipNear * std::tan(gb::math::degree2radian(fov));
	left = - right;

	top = right * aspectRation;
	bottom = - top;

	sphereBB = genSphereBB<T>();
	// updateAABB();
	
	projectionMatrix = perspectiveProjectionMatrix<T>();
    }

    mat4<T> perspectiveProjectionMatrix() const
    {
	/*
	  1st: projection map
	  2nd: linear map form [l, r] to [-1, 1] (map to NDC)

	  NOTE: camera'z axis is backward to the object,
	  for the reaseon of having the same right-hand x-y-z coordinate system 
	  as world space.

          1st:
       		     ------z-----x
       	       	      \	   |   	/
     		       \   |   /
    			---n--x'
   			 \ | /
  			  \|/
    			 camera
          
          x'/x = -n/z => x' = -(n/z) * x;

	  2nd: map form [l, r] to [-1, 1]
	  (x-l)/(r-l) = (x'+1)/(1+1) 
	  =>x' = 2(x-l)/(r-l) - 1 = 2x/(r-l) - (2l+r-l)/(r-l) = 2x/(r-l) - (l+r)/(r-l)

	  substitue x with -(n/z) * x
	  =>(1/-z)(2n/(r-l))x - (l+r)/(r-l)

	  because opengl will divide all x, y, z by w in (x, y, z, w) homogeneous coordinate
	  so final coordinate will be(xw, yw, zw, w)
	  let w = -z, then
	  x' = (2n/(r-l))x + ((l+r)/(r-l))z
	  and, y' = (2n/(t-b))y + ((b+t)/(t-b))z

	  so, 1st row of matrix will be (2n/(r-l), 0, (l+r)/(r-l), 0)
	  and 2nd row will be (0, 2n/(t-b), (b+t)/(t-b), 0)

	  suppose 3rd row is (0, 0, A, B), 
	  then z' = Az + B, and z_final = z'/-z = -A - B/z
	  linear map z_final form [-f, -n] to [1, -1] (not [-1, 1], since Zfar > Znear)
	  =>
	  1 = -A + B/f, -1 = -A + B/n
	  => 2 = B/n - B/f = B(n-f)/fn
	  => B = 2fn/(n-f)
	  substitue for 1 = A + B/f
	  => 1 = -A + 2n/(n-f)
	  => A = n-f-2n/(n-f) = (f+n)/(n-f)

	  so the matrix is
	  (2n/(r-l), 0, (l+r)/(r-l), 0)
	  (0, 2n/(t-b), (b+t)/(t-b), 0)
	  (0, 0, (f+n)/(n-f), 2fn/(n-f))
	  (0, 0, -1, 0)
	 */
	mat4<T> ret;
	ret[0][0] = 2 * clipNear / (right - left);
	ret[2][0] = (left + right) / (right - left);
	
	ret[1][1] = 2 * clipNear / (top - bottom);
	ret[2][1] = (bottom + top) / (top - bottom);

	ret[2][2] = (clipFar + clipNear) / (clipNear - clipFar);
	ret[3][2] = 2 * clipFar * clipNear / (clipNear - clipFar);

	ret[2][3] = -1;

	return ret;
    }

    /*
      orthographic frustum
     */
    set(const T left_,
	    const T right_,
	    const T bottom_,
	    const T top_,
	    const T clipNear_,
	    const T clipFar_)
    {
	left = left_;
	right = right_;
	bottom = bottom_;
	top = top_;
	clipNear = clipNear_;
	clipFar = clipFar_;

	sphereBB = genSphereBB<T>();
	//updateAABB();
	
	projectionMatrix = orthographicProjectionMatrix<T>();
    }
    
    /*
      orthographic projection
      there's only one step to do here, which is linear map [l, r] to [-1, 1]
      
      (x - l)/(r - l) = (x' + 1)/2
      =>x' = (2x - 2l -r + l)/(r-l) = 2x/(r-l) + (l+r)/(l-r);
      so 1st row of matrix would be
      (2/(r-l), 0, 0, (l+r)/(l-r))
      and 2nd
      (0, 2/(t-b), 0, (b+t)/(b-t))
      3rd([-n, -f] to [-1, 1], see perspective projection)
      (0, 0, 2/(n-f), (n+f)/(n-f))
     */
    mat4<T> orthographicProjectionMatrix()
    {
	mat4<T> ret;
	ret[0][0] = 2 / (right - left);
	ret[3][0] = (left + right) / (left - right);

	ret[1][1] = 2 / (top - bottom);
	ret[3][1] = (bottom + top) / (bottom - top);

	ret[2][2] = 2 / (clipNear - clipFar);
	ret[3][2] = (clipNear + clipFar) / (clipNear - clipFar);

	ret[3][3] = 1;

	return ret;
    }

    spherebb<T> genSphereBB() const
    	{
    	    T tmp = (right - left) / 2;
    	    T radius = tmp;
    	    tmp = (top - bottom) / 2;
    	    if(tmp > radius)
    		radius = tmp;

    	    tmp = (clipFar - clipNear) / 2;
    	    if(tmp > radius)
    		radius = tmp;

    	    return spherebb<T>(vec3<T>(), radius);
    	}

    // void updateAABB()
    // 	{
    // 	    AABB.set(vec3<T>(left, bottom, clipNear), vec3<T>(right, top, clipFar));
    // 	}
    
    T left, right, bottom, top, clipNear, clipFar;
    mat4<T> projectionMatrix;
    spherebb<T> sphereBB;
    //aabb<T> AABB;
};



GB_PHYSICS_NS_END
