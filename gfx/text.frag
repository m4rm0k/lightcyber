/* Endeavor by Team210 - 64k intro by Team210 at Revision 2k19
 * Copyright (C) 2019  Alexander Kraus <nr4@z10.info>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
 
#version 130

uniform float iFontWidth, iTime;
uniform vec2 iResolution;
uniform sampler2D iChannel0, iFont;

out vec4 gl_FragColor;

// Global constants
const vec3 c = vec3(1.,0.,-1.);
const float pi = acos(-1.);
float a; // Aspect ratio

void rand(in vec2 x, out float num);
void lfnoise(in vec2 t, out float num);
void rshort(in float off, out float val);
void rfloat(in float off, out float val);
void dbox(in vec2 x, in vec2 b, out float dst);
void dcircle(in vec2 x, out float d);
void dlinesegment(in vec2 x, in vec2 p1, in vec2 p2, out float d);
void drhomboid(in vec2 x, in vec2 b, in float tilt, out float dst);
void dcirclesegment(in vec2 x, in float r, in float p0, in float p1, out float d);
void stroke(in float d0, in float s, out float d);
void dglyph(in vec2 x, in float ordinal, in float size, out float dst);
void dstring(in vec2 x, in float ordinal, in float size, out float dst);
void dfloat(in vec2 x, in float num, in float size, out float dst);
void smoothmin(in float a, in float b, in float k, out float dst);
void dint(in vec2 x, in float num, in float size, in float ndigits, out float dst);
void dtime(in vec2 x, in float num, in float size, out float dst);

// Fixme: remove vec4 technique in favor of separate distance
// void blendadd(in vec4 src1, in vec4 src2, in float tlo, in float thi, out vec4 dst)
// {
//     vec4 added;
//     add(src1, src2, added);
//     dst = mix(src1, added, smoothstep(tlo-.5,tlo+.5,iTime)*(1.-smoothstep(thi-.5,thi+.5,iTime)));
// }

void window(in vec2 x, in vec2 size, in vec3 bg, in float title_index, out vec4 col);
void progressbar(in vec2 x, in float width, in float progress, out vec4 col);

float sm(float d)
{
    return smoothstep(1.5/iResolution.y, -1.5/iResolution.y, d);
}

void dvoronoi(in vec2 x, out float d, out vec2 z);
void colorize(in vec2 x, out vec3 col)
{
    vec3 c1;
    vec2 ind,
        xv,
        xi;
    float d,
        vs = 16.,
        n,
        size = .1,
        xix = mod(x.x, size)-.5*size,
        xixj = (x.x - xix),
        ri,
        rim1,
        rip1,
        lines = 8.,
        da,
        op,
        s;
    
    // Background blending
    s = smoothstep(0.,.5,.5-abs(x.y));
    col = mix(1.e-4*c.xxx, vec3(0.04,0.18,0.24), s);
    
    // Background circles
    dvoronoi(vs*x, d, ind);
    xv = ind/vs-x;
    lfnoise(vec2(3.,33.)*ind/vs-3.*iTime*c.xy,n);
    n = .5+.5*n;
    d = length(xv)-mix(.0,.35,n)/vs;
    col = mix(col, n*.5*vec3(1.00,0.40,0.39), sm(d));
    d = abs(d-.005) -.002;
    col = mix(col, (1.-n)*vec3(0.49,0.71,0.78), sm(d));
    
    for(float i = 1.; i < 9.; i += 1.)
    {
        rand((9.-i)*c.xx, op);
        op = .5+.5*round(16.*op)/16.;
        x += -.1+.2*op;
        
        xix = mod(x.x, size)-.5*size;
        xixj = (x.x - xix);
        
        // Edges
        lfnoise(2.e0*xixj*c.xx+14.*i, ri);
        lfnoise(2.e0*(xixj+size)*c.xx+14.*i, rip1);
        lfnoise(2.e0*(xixj-size)*c.xx+14.*i, rim1);

        float h = .2;
        
        ri = h*round(lines*ri)/lines;
        rip1 = h*round(lines*rip1)/lines;
        rim1 = h*round(lines*rim1)/lines;

        //if(ri < 0.)
        {
            dlinesegment(vec2(xix, x.y), vec2(-.5*size, mix(ri,rim1,.5)), vec2(-.25*size, ri), d);
            dlinesegment(vec2(xix, x.y), vec2(-.25*size, ri), vec2(.25*size, ri), da);
            d = min(d, da);
            dlinesegment(vec2(xix, x.y), vec2(.25*size, ri), vec2(.5*size, mix(ri,rip1,.5)), da);
            d = min(d, da);
            stroke(d, .002+.002*op, d);
            col = mix(col, op*(1.-n)*vec3(1.00,0.40,0.39), sm(d));

            // Dots
            lfnoise(8.*xixj*c.xx-3.*iTime*c.xy+14.*i, n);
            n = .5+.5*n;
            d = length(vec2(xix, x.y-ri))-mix(.0,.35,n)/vs;
            c1 = mix(vec3(1.00,0.40,0.39), vec3(0.85,0.87,0.89), n);
            col = mix(col, op*(1.-n)*c1, sm(d));
            stroke(d - .009, (1.-n)*.005, d);
            c1 *= 2.4;
            col = mix(col, op*(1.-n)*c1, sm(d));
        }
        
        x -= -.1+.2*op;
    }
    
    //mix to blackish
    lfnoise(3.*x.xy-vec2(1.,.1)*iTime, n);
    stroke(n, .3, n);
    col = mix(col, 1.e-4*c.xxx, n);
    col = mix(col, .1*col, 1.-s);
    
    col = mix(col, mix(col, vec3(1.00,0.40,0.39), mix(.4,.8,.5+.5*x.y/.1)), sm(abs(x.y)-.1));
    col = mix(col, c.xxx, sm(abs(abs(x.y)-.11)-.001));
    
    col = mix(col, col*col, clamp(-x.y/.1,0.,1.));
    col *= col;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    a = iResolution.x/iResolution.y;
    vec2 uv = fragCoord/iResolution.yy-0.5*vec2(a, 1.0);
    
    float d;

    vec4 old = vec4(-1.,texture(iChannel0, fragCoord/iResolution.xy).rgb), 
    new = old; // Scene
    
    if(uv.y < -.3)
    {
        // Add overlay
        colorize(2.*(c.xz*uv-.45*vec2(-a,1.)-12.*c.xy), new.gba);
        new.gba = mix(old.gba, mix(old.gba, new.gba,.4), smoothstep(3.e-2, 5.e-2,length(new.gba)));
    }
    
    if(uv.y > .4)
    {
        // Add Static text
        dstring((uv-.45*vec2(-.85*a,1.)), 3., .02, d); // Team210

        new.gba = mix(new.gba, mix(new.gba, c.xxx, .5), sm(d));
        
        stroke(d-.002, .001, d);
        new.gba = mix(new.gba, vec3(1.00,0.40,0.39), sm(d));

        // Add time overlay
        dtime((uv-.45*vec2(1.*a,1.05)), iTime, .01, d);
        new.gba = mix(new.gba, c.xxx, sm(d));
    }
    
    if(iTime < 6.)
    {
        vec2 dx = (.25*a+.3*c.xy)*c.xy;
        if(iTime < 3.)
        {
            float ind = mix(100000., 2., clamp(iTime/3.,0.,1));
            dint(uv+dx*c.xy, ind, .02, 6., d);
        }
        else if(iTime < 4.)
        {
            dint(uv+dx, 2., .02, 6., d);
        }
        else if(iTime < 5.)
        {
            dint(uv+dx, 1., .02, 6., d);
        }
        else if(iTime < 6.)
        {
            dint(uv+dx, 0., .02, 6., d);
        }
        
        float da;
        dstring(uv+dx-2.*9.*.02*c.xy, 4., .02, da);
        d = min(d, da);
            
        new.gba = mix(new.gba, mix(new.gba, vec3(1.00,0.87,0.57), .7), sm(d));
        stroke(d-.002, .001, d);
        new.gba = mix(new.gba, c.xxx, sm(d));
        
        
    }
    
    fragColor = vec4(new.gba, 1.);
}

void main()
{
    mainImage(gl_FragColor, gl_FragCoord.xy);
}
