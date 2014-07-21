/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "CCOBB.h"

NS_CC_BEGIN

#define ROTATE(a,i,j,k,l) g=a.m[i + 4 * j]; h=a.m[k + 4 * l]; a.m[i + 4 * j]=(float)(g-s*(h+g*tau)); a.m[k + 4 * l]=(float)(h+s*(g-h*tau));

static Mat4 _convarianceMatrix(const Vec3* aVertPos, int nVertCount)
{
    int i;
    Mat4 Cov;

    double S1[3];
    double S2[3][3];

    S1[0] = S1[1] = S1[2] = 0.0;
    S2[0][0] = S2[1][0] = S2[2][0] = 0.0;
    S2[0][1] = S2[1][1] = S2[2][1] = 0.0;
    S2[0][2] = S2[1][2] = S2[2][2] = 0.0;

    // get center of mass
    for(i=0; i<nVertCount; i++)
    {
        S1[0] += aVertPos[i].x;
        S1[1] += aVertPos[i].y;
        S1[2] += aVertPos[i].z;

        S2[0][0] += aVertPos[i].x * aVertPos[i].x;
        S2[1][1] += aVertPos[i].y * aVertPos[i].y;
        S2[2][2] += aVertPos[i].z * aVertPos[i].z;
        S2[0][1] += aVertPos[i].x * aVertPos[i].y;
        S2[0][2] += aVertPos[i].x * aVertPos[i].z;
        S2[1][2] += aVertPos[i].y * aVertPos[i].z;
    }

    float n = (float)nVertCount;
    // now get covariances
    Cov.m[0] = (float)(S2[0][0] - S1[0]*S1[0] / n) / n;
    Cov.m[5] = (float)(S2[1][1] - S1[1]*S1[1] / n) / n;
    Cov.m[10] = (float)(S2[2][2] - S1[2]*S1[2] / n) / n;
    Cov.m[4] = (float)(S2[0][1] - S1[0]*S1[1] / n) / n;
    Cov.m[9] = (float)(S2[1][2] - S1[1]*S1[2] / n) / n;
    Cov.m[8] = (float)(S2[0][2] - S1[0]*S1[2] / n) / n;
    Cov.m[1] = Cov.m[4];
    Cov.m[2] = Cov.m[8];
    Cov.m[6] = Cov.m[9];

    return Cov;
}

static float& _getElement( Vec3& point, int index)
{
    if (index == 0)
        return point.x;
    if (index == 1)
        return point.y;
    if (index == 2)
        return point.z;

    CC_ASSERT(0);
    return point.x;
}

static void _eigenVectors(Mat4* vout, Vec3* dout, Mat4 a)
{
    int n = 3;
    int j,iq,ip,i;
    double tresh,theta,tau,t,sm,s,h,g,c;
    int nrot;
    Vec3 b;
    Vec3 z;
    Mat4 v;
    Vec3 d;

    v = Mat4::IDENTITY;
    for(ip=0; ip<n; ip++)
    {
        _getElement(b, ip) = a.m[ip + 4 * ip];
        _getElement(d, ip) = a.m[ip + 4 * ip];
        _getElement(z, ip) = 0.0;
    }

    nrot = 0;

    for(i=0; i<50; i++)
    {
        sm=0.0;
        for(ip=0; ip<n; ip++) for(iq=ip+1;iq<n;iq++) sm+=fabs(a.m[ip + 4 * iq]);
        if( sm == 0.0 )
        {
            v.transpose();
            *vout = v;
            *dout = d;
            return;
        }

        if (i < 3) tresh = 0.2 * sm / (n*n);
        else tresh=0.0;

        for(ip=0; ip<n; ip++)
        {
            for(iq=ip+1; iq<n; iq++)
            {
                g = 100.0*fabs(a.m[ip + iq * 4]);

                float dmip = _getElement(d, ip);
                float dmiq = _getElement(d, iq);
                if( i>3 && fabs(dmip)+g==fabs(dmip) && fabs(dmiq)+g==fabs(dmiq) )
                    a.m[ip + 4 * iq]=0.0;
                else if (fabs(a.m[ip + 4 * iq])>tresh)
                {
                    h = dmiq-dmip;
                    if (fabs(h)+g == fabs(h))
                        t=(a.m[ip + 4 * iq])/h;
                    else
                    {
                        theta=0.5*h/(a.m[ip + 4 * iq]);
                        t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
                        if (theta < 0.0) t = -t;
                    }
                    c=1.0/sqrt(1+t*t);
                    s=t*c;
                    tau=s/(1.0+c);
                    h=t*a.m[ip + 4 * iq];
                    _getElement(z, ip) -= (float)h;
                    _getElement(z, iq) += (float)h;
                    _getElement(d, ip) -= (float)h;
                    _getElement(d, iq) += (float)h;
                    a.m[ip + 4 * iq]=0.0;
                    for(j=0;j<ip;j++) { ROTATE(a,j,ip,j,iq); }
                    for(j=ip+1;j<iq;j++) { ROTATE(a,ip,j,j,iq); }
                    for(j=iq+1;j<n;j++) { ROTATE(a,ip,j,iq,j); }
                    for(j=0;j<n;j++) { ROTATE(v,j,ip,j,iq); }
                    nrot++;
                }
            }
        }
        for(ip=0;ip<n;ip++)
        {
            _getElement(b, ip) += _getElement(z, ip);
            _getElement(d, ip) = _getElement(b, ip);
            _getElement(z, ip) = 0.0f;
        }
    }

    v.transpose();
    *vout = v;
    *dout = d;
    return;
}

static Mat4 _GetOBBOrientation(const Vec3* aVertPos, int nVertCount)
{
    Mat4 Cov;

    if (nVertCount <= 0)
        return Mat4::IDENTITY;

    Cov = _convarianceMatrix(aVertPos, nVertCount);

    // now get eigenvectors
    Mat4 Evecs;
    Vec3 Evals;
    _eigenVectors(&Evecs, &Evals, Cov);

    Evecs.transpose();

    return Evecs;
}

OBB::OBB()
{
    reset();
}

OBB::OBB(const AABB& aabb)
{
    reset();
    
    _center = (aabb._min + aabb._max);
    _center.scale(0.5f);
    _xAxis = Vec3(1.0f, 0.0f, 0.0f);
    _yAxis = Vec3(0.0f, 1.0f, 0.0f);
    _zAxis = Vec3(0.0f, 0.0f, 1.0f);
    
    _extents = aabb._max - aabb._min;
    _extents.scale(0.5f);
}

OBB::OBB(const Vec3* verts, int nVerts)
{
    if (!nVerts) return;
    
    reset();
    
    Mat4 matTransform = _GetOBBOrientation(verts, nVerts);
    
    //	For matTransform is orthogonal, so the inverse matrix is just rotate it;
    matTransform.transpose();
    
    Vec3 vecMax = matTransform * Vec3(verts[0].x, verts[0].y, verts[0].z);
    
    Vec3 vecMin = vecMax;
    
    for (int i=1; i < nVerts; i++)
    {
        Vec3 vect = matTransform * Vec3(verts[i].x, verts[i].y, verts[i].z);
        
        vecMax.x = vecMax.x > vect.x ? vecMax.x : vect.x;
        vecMax.y = vecMax.y > vect.y ? vecMax.y : vect.y;
        vecMax.z = vecMax.z > vect.z ? vecMax.z : vect.z;
        
        vecMin.x = vecMin.x < vect.x ? vecMin.x : vect.x;
        vecMin.y = vecMin.y < vect.y ? vecMin.y : vect.y;
        vecMin.z = vecMin.z < vect.z ? vecMin.z : vect.z;
    }
    
    matTransform.transpose();
    
    _xAxis = Vec3(matTransform.m[0], matTransform.m[1], matTransform.m[2]);
    _yAxis = Vec3(matTransform.m[4], matTransform.m[5], matTransform.m[6]);
    _zAxis = Vec3(matTransform.m[8], matTransform.m[9], matTransform.m[10]);
    
    _center	= 0.5f * (vecMax + vecMin);
    _center *= matTransform;
    
    _xAxis.normalize();
    _yAxis.normalize();
    _zAxis.normalize();
    
    _extents = 0.5f * (vecMax - vecMin);
}

bool OBB::isPointIn(const Vec3& point) const
{
    Vec3 vd = point - _center;

    float d = vd.dot(_xAxis);
    if (d > _extents.x || d < -_extents.x)
        return false;

    d = vd.dot(_yAxis);
    if (d > _extents.y || d < -_extents.y)
        return false;

    d = vd.dot(_zAxis);
    if (d > _extents.z || d < -_extents.z)
        return false;

    return true;
}

void OBB::set(const Vec3& center, const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis, const Vec3& extents)
{
    _center = center;
    _xAxis = xAxis;
    _yAxis = yAxis;
    _zAxis = zAxis;
    _extents = extents;
}

void OBB::reset()
{
    memset(this, 0, sizeof(OBB));
}

void OBB::getCorners(Vec3* verts) const
{
    Vec3 extX = _xAxis * _extents.x;
    Vec3 extY = _yAxis * _extents.y;
    Vec3 extZ = _zAxis * _extents.z;
    
    verts[0] = _center - extX + extY + extZ;     // left top front
    verts[1] = _center - extX - extY + extZ;     // left bottom front
    verts[2] = _center + extX - extY + extZ;     // right bottom front
    verts[3] = _center + extX + extY + extZ;     // right top front
    
    verts[4] = _center + extX + extY - extZ;     // right top back
    verts[5] = _center + extX - extY - extZ;     // right bottom back
    verts[6] = _center - extX - extY - extZ;     // left bottom back
    verts[7] = _center - extX + extY - extZ;     // left top back
}

void OBB::transform(const Mat4& mat)
{
    Vec4 newcenter = mat * Vec4(_center.x, _center.y, _center.z, 1.0f);// center;
    _center.x = newcenter.x;
    _center.y = newcenter.y;
    _center.z = newcenter.z;

    _xAxis = mat * _xAxis;
    _yAxis = mat * _yAxis;
    _zAxis = mat * _zAxis;

    _xAxis.normalize();
    _yAxis.normalize();
    _zAxis.normalize();

    Vec3 scale, trans;
    Quaternion quat;
    mat.decompose(&scale, &quat, &trans);

    _extents.x *= scale.x;
    _extents.y *= scale.y;
    _extents.z *= scale.z;
}

NS_CC_END